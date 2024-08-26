#include "reflection_renderer.hpp"

#include <stdint.h>

#include <initializer_list>
#include <queue>
#include <thread>
#include <unordered_set>

#include "dds_loader.hpp"
#include "engine.hpp"
#include "entity.hpp"
#include "gpu_mesh_buffer.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "matrix_utility.hpp"
#include "mesh.hpp"
#include "reflection_renderer_pass.hpp"
#include "transform.hpp"
#include "vector.hpp"

using namespace SF;

void ReflectionRenderer::initialize() {
  // create depth texture for passes
  {
    auto formats = {wgpu::TextureFormat::Depth24Plus};
    wgpu::TextureDescriptor depth_texture_desc{
        .nextInChain = nullptr,
        .usage = wgpu::TextureUsage::RenderAttachment,
        .dimension = wgpu::TextureDimension::e2D,
        .size = {1080, 1080, 1},
        .format = wgpu::TextureFormat::Depth24Plus,
        .mipLevelCount = 1,
        .sampleCount = 1,
        .viewFormatCount = 1,
        .viewFormats = formats.begin(),
    };

    depth_texture = Engine::get_module<Graphics>()->get_device().CreateTexture(
        &depth_texture_desc);
  }

  diffuse_pass =
      create_diffuse_pass(*Engine::get_module<Graphics>(), depth_texture);

  aabb_pass = create_aabb_pass(*Engine::get_module<Graphics>());

  texture_pass = create_texture_pass(*Engine::get_module<Graphics>());

  // set up sample data
  {
    auto ratio = 1080.0f / 1080.0f;
    auto focal_length = 2.0f;
    auto near = 0.01f;
    auto far = 100.0f;
    auto divides = 1.0f / (focal_length * (far - near));
    auto view_proj_mat =
        Math::perspective_projection(ratio, focal_length, near, far);
    auto view_proj_mat_vec = std::vector<float>();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        view_proj_mat_vec.push_back(view_proj_mat.internal_data.at(j).at(i));
      }
    }

    // camera buffer
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "camera buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
        .size = Engine::get_module<Graphics>()->get_buffer_stride(
            sizeof(Math::Matrix4x4f)),
        .mappedAtCreation = false};

    camera_buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(camera_buffer,
                                                  view_proj_mat_vec);

    // camera constant bind group
    auto constant_binding = wgpu::BindGroupEntry{.binding = 0,
                                                 .buffer = camera_buffer,
                                                 .offset = 0,
                                                 .size = sizeof(float) * 16};

    wgpu::BindGroupDescriptor bind_group_desc{
        .layout = diffuse_pass.camera_constant_bind_group_layout,
        .entryCount = 1,
        .entries = &constant_binding};

    camera_constant_bind_group =
        Engine::get_module<Graphics>()->get_device().CreateBindGroup(
            &bind_group_desc);

    // dummy texture
    std::vector<uint8_t> texture_data(1 * 1 * 4, 0xff);
    const wgpu::TextureDescriptor texture_desc{
        .nextInChain = nullptr,
        .usage =
            wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding,
        .dimension = wgpu::TextureDimension::e2D,
        .size = {1, 1, 1},
        .format = wgpu::TextureFormat::RGBA8Unorm,
        .mipLevelCount = 1,
        .sampleCount = 1,
        .viewFormatCount = 0,
        .viewFormats = nullptr,
    };

    texture = Engine::get_module<Graphics>()->get_device().CreateTexture(
        &texture_desc);

    wgpu::ImageCopyTexture dest{
        .texture = texture,
        .mipLevel = 0,
        .origin = {0, 0, 0},
        .aspect = wgpu::TextureAspect::All,
    };

    wgpu::TextureDataLayout source{
        .offset = 0,
        .bytesPerRow = 4 * 1,
        .rowsPerImage = 1,
    };

    Engine::get_module<Graphics>()->get_device().GetQueue().WriteTexture(
        &dest, texture_data.data(), texture_data.size(), &source,
        &texture_desc.size);

    auto texture_view_desc = wgpu::TextureViewDescriptor{
        .format = wgpu::TextureFormat::RGBA8Unorm,
        .dimension = wgpu::TextureViewDimension::e2D,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
        .aspect = wgpu::TextureAspect::All,
    };

    texture_view = texture.CreateView(&texture_view_desc);

    auto sampler_desc = wgpu::SamplerDescriptor{
        .addressModeU = wgpu::AddressMode::Repeat,
        .addressModeV = wgpu::AddressMode::Repeat,
        .addressModeW = wgpu::AddressMode::Repeat,
        .magFilter = wgpu::FilterMode::Linear,
        .minFilter = wgpu::FilterMode::Linear,
        .mipmapFilter = wgpu::MipmapFilterMode::Linear,
        .lodMinClamp = 0.0f,
        .lodMaxClamp = 0.0f,
        .compare = wgpu::CompareFunction::Undefined,
        .maxAnisotropy = 1,
    };
    sampler = Engine::get_module<Graphics>()->get_device().CreateSampler(
        &sampler_desc);

    auto texture_bindings = std::vector<wgpu::BindGroupEntry>(2);
    texture_bindings.at(0) = wgpu::BindGroupEntry{
        .binding = 0,
        .textureView = texture_view,
    };
    texture_bindings.at(1) = wgpu::BindGroupEntry{
        .binding = 1,
        .sampler = sampler,
    };

    wgpu::BindGroupDescriptor texture_bind_group_desc{
        .layout = diffuse_pass.texture_bind_group_layout,
        .entryCount = static_cast<uint32_t>(texture_bindings.size()),
        .entries = texture_bindings.data(),
    };

    texture_bind_group =
        Engine::get_module<Graphics>()->get_device().CreateBindGroup(
            &texture_bind_group_desc);

    // gizmo vertex buffer
    float width = 0.005f;
    std::vector<GizmoVertex> gizmo_vertex{
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f + width, 1.0f + width, 1.0f + width, 1.0f}),
            .color = Math::Vector3f({1.0f, 0.0f, 0.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f + width, -1.0f - width, 1.0f + width, 1.0f}),
            .color = Math::Vector3f({0.0f, 1.0f, 0.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f - width, 1.0f - width, 1.0f - width, 1.0f}),
            .color = Math::Vector3f({1.0f, 0.0f, 0.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f - width, -1.0f + width, 1.0f - width, 1.0f}),
            .color = Math::Vector3f({0.0f, 1.0f, 0.0f}),
        },

        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f - width, 1.0f + width, 1.0f + width, 1.0f}),
            .color = Math::Vector3f({0.0f, 0.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f - width, -1.0f - width, 1.0f + width, 1.0f}),
            .color = Math::Vector3f({1.0f, 1.0f, 0.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f + width, 1.0f - width, 1.0f - width, 1.0f}),
            .color = Math::Vector3f({0.0f, 0.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f + width, -1.0f + width, 1.0f - width, 1.0f}),
            .color = Math::Vector3f({1.0f, 1.0f, 0.0f}),
        },

        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f + width, 1.0f + width, -1.0f - width, 1.0f}),
            .color = Math::Vector3f({1.0f, 0.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f + width, -1.0f - width, -1.0f - width, 1.0f}),
            .color = Math::Vector3f({0.0f, 1.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f - width, 1.0f - width, -1.0f + width, 1.0f}),
            .color = Math::Vector3f({1.0f, 0.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {1.0f - width, -1.0f + width, -1.0f + width, 1.0f}),
            .color = Math::Vector3f({0.0f, 1.0f, 1.0f}),
        },

        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f - width, 1.0f + width, -1.0f - width, 1.0f}),
            .color = Math::Vector3f({1.0f, 1.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f - width, -1.0f - width, -1.0f - width, 1.0f}),
            .color = Math::Vector3f({0.0f, 0.0f, 0.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f + width, 1.0f - width, -1.0f + width, 1.0f}),
            .color = Math::Vector3f({1.0f, 1.0f, 1.0f}),
        },
        GizmoVertex{
            .position = Math::Vector4f(
                {-1.0f + width, -1.0f + width, -1.0f + width, 1.0f}),
            .color = Math::Vector3f({0.0f, 0.0f, 0.0f}),
        },
    };

    const wgpu::BufferDescriptor gizmo_vertex_buffer_desc{
        .nextInChain = nullptr,
        .label = "gizmo vertex buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
        .size = sizeof(GizmoVertex) * gizmo_vertex.size(),
        .mappedAtCreation = false};

    gizmo_vertex_buffer =
        Engine::get_module<Graphics>()->create_buffer(gizmo_vertex_buffer_desc);

    Engine::get_module<Graphics>()->update_buffer(gizmo_vertex_buffer,
                                                  gizmo_vertex);

    // gizmo index buffer
    std::vector<uint32_t> gizmo_index{
        0,  1,  2,  2,  1,  3,  4, 5,  6,  6,  5,  7,  8, 9, 10, 10, 9, 11,
        12, 13, 14, 14, 13, 15, 0, 2,  4,  4,  2,  6,  1, 5, 3,  3,  5, 7,
        8,  10, 12, 12, 10, 14, 9, 11, 13, 13, 11, 15, 4, 6, 12, 12, 6, 14,
        5,  7,  13, 13, 7,  15, 0, 2,  8,  8,  2,  10, 1, 3, 9,  9,  3, 11,
    };

    const wgpu::BufferDescriptor gizmo_index_buffer_desc{
        .nextInChain = nullptr,
        .label = "gizmo index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = sizeof(uint32_t) * gizmo_index.size(),
        .mappedAtCreation = false,
    };

    gizmo_index_buffer =
        Engine::get_module<Graphics>()->create_buffer(gizmo_index_buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(gizmo_index_buffer,
                                                  gizmo_index);

    // gizmo camera constant bind group
    auto gizmo_camera_constant_binding = wgpu::BindGroupEntry{
        .binding = 0,
        .buffer = camera_buffer,
        .offset = 0,
        .size = sizeof(float) * 16,
    };

    wgpu::BindGroupDescriptor gizmo_camera_bind_group_desc{
        .layout = diffuse_pass.camera_constant_bind_group_layout,
        .entryCount = 1,
        .entries = &gizmo_camera_constant_binding,
    };

    gizmo_camera_bind_group =
        Engine::get_module<Graphics>()->get_device().CreateBindGroup(
            &gizmo_camera_bind_group_desc);
  }

  {
    // create texture pass index buffer
    std::vector<uint32_t> texture_pass_index{
        0, 1, 2, 2, 1, 3,
    };

    const wgpu::BufferDescriptor texture_pass_index_buffer_desc{
        .nextInChain = nullptr,
        .label = "texture pass index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = sizeof(uint32_t) * texture_pass_index.size(),
        .mappedAtCreation = false,
    };

    texture_pass_index_buffer = Engine::get_module<Graphics>()->create_buffer(
        texture_pass_index_buffer_desc);

    Engine::get_module<Graphics>()->update_buffer(texture_pass_index_buffer,
                                                  texture_pass_index);

    // create texture pass test bind group
    std::vector<wgpu::BindGroupEntry> texture_bindings{
        wgpu::BindGroupEntry{
            .binding = 0,
            .textureView = texture_view,
        },
        wgpu::BindGroupEntry{
            .binding = 1,
            .sampler = sampler,
        },
    };

    wgpu::BindGroupDescriptor texture_pass_test_bind_group_desc{
        .layout = texture_pass.texture_bind_group_layout,
        .entryCount = 2,
        .entries = texture_bindings.data(),
    };

    texture_pass_test_bind_group =
        Engine::get_module<Graphics>()->get_device().CreateBindGroup(
            &texture_pass_test_bind_group_desc);
  }

  // compute shader test
  {}
}

void ReflectionRenderer::init_aabb_data() {
  // gizmo constant buffer
  auto gizmo_constant_buffer_desc = wgpu::BufferDescriptor{
      .nextInChain = nullptr,
      .label = "gizmo constant buffer",
      .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
      .size = Engine::get_module<Graphics>()->get_buffer_stride(sizeof(AABB)) *
              aabb_count,
      .mappedAtCreation = false,
  };

  gizmo_constant_buffer =
      Engine::get_module<Graphics>()->create_buffer(gizmo_constant_buffer_desc);
  auto stride = Engine::get_module<Graphics>()->get_buffer_stride(sizeof(AABB));
  for (int i = 0; i < aabb_count; i++) {
    auto mat = std::vector<float>{
        -1.0f + i, -1.0f + i, -1.0f + i, 0.0f,
        1.0f + i,  1.0f + i,  1.0f + i,  0.0f,
    };
    Engine::get_module<Graphics>()->update_buffer(gizmo_constant_buffer, mat,
                                                  i * stride);
  }

  auto gizmo_constant_binding = wgpu::BindGroupEntry{
      .binding = 0,
      .buffer = gizmo_constant_buffer,
      .offset = 0,
      .size = sizeof(AABB),
  };

  wgpu::BindGroupDescriptor gizmo_bind_group_desc{
      .layout = aabb_pass.aabb_data_bind_group_layout,
      .entryCount = 1,
      .entries = &gizmo_constant_binding,
  };

  gizmo_constant_bind_group =
      Engine::get_module<Graphics>()->get_device().CreateBindGroup(
          &gizmo_bind_group_desc);
};

void ReflectionRenderer::render(wgpu::TextureView render_target) {
  if (render_bundle == nullptr) {
    // update constants
    auto entity_list = Engine::get_module<EntityStore>()->get_all();

    // create gpu resources
    uint64_t vertex_count;
    uint64_t vertex_offset;
    uint64_t index_count;
    uint64_t index_offset;

    std::unordered_set<std::filesystem::path> added_textures;
    for (const auto& entity : entity_list) {
      {
        // mesh buffer
        const auto& mesh = entity->get_component<Mesh>();
        const auto& transform = entity->get_component<Transform>();

        auto entity_id = entity->get_id();
        if (mesh == nullptr || transform == nullptr) {
          dispose_gpu_resource(entity_id);
          continue;
        }

        if (!gpu_resources.contains(entity_id)) {
          MeshBuffer resource{};
          auto mesh_buffer = create_mesh_buffer(entity_id);
          resource.vertex_buffer = std::get<0>(mesh_buffer);
          resource.index_buffer = std::get<1>(mesh_buffer);
          resource.transform_buffer = create_constant_buffer(entity_id);

          gpu_resources.insert_or_assign(entity_id, resource);
        }
      }

      {
        // texture buffer
        const auto& material = entity->get_component<Material>();
        const auto& texture_path = material->texture_path;

        added_textures.insert(texture_path);
      }
    }

    // load
    {
      std::queue<std::function<void()>> load_tasks;
      std::mutex task_mutex;

      for (auto& texture_path : added_textures) {
        load_tasks.push([this, texture_path]() { load_texture(texture_path); });
      }

      std::vector<std::thread> load_threads;
      for (int i = 0; i < std::thread::hardware_concurrency(); i++) {
        load_threads.push_back(std::thread([&load_tasks, &task_mutex]() {
          while (true) {
            task_mutex.lock();
            if (load_tasks.empty()) {
              task_mutex.unlock();
              break;
            }
            auto task = load_tasks.front();
            load_tasks.pop();
            task_mutex.unlock();
            task();
          }
        }));
      }
      for (auto& thread : load_threads) {
        thread.join();
      }
    }

    // set is_transparent
    for (const auto& entity : entity_list) {
      const auto& material = entity->get_component<Material>();
      const auto& texture_path = material->texture_path;
      material->is_transparent = textures.at(texture_path).is_transparent;
    }

    if (!aabb_initialized) {
      aabb_initialized = true;
      aabb_count = entity_list.size();
      init_aabb_data();

      auto stride =
          Engine::get_module<Graphics>()->get_buffer_stride(sizeof(AABB));
      std::cout << "create aabb" << std::endl;
      for (int i = 0; i < aabb_count; i++) {
        const auto& mesh = entity_list.at(i)->get_component<Mesh>();
        const auto& transform = entity_list.at(i)->get_component<Transform>();
        AABB aabb{
            .min = Math::Vector3f({std::numeric_limits<float>::max(),
                                   std::numeric_limits<float>::max(),
                                   std::numeric_limits<float>::max()}),
            .max = Math::Vector3f({std::numeric_limits<float>::lowest(),
                                   std::numeric_limits<float>::lowest(),
                                   std::numeric_limits<float>::lowest()})};
        for (const auto& vertex : mesh->get_vertices()) {
          aabb.min.x = std::min(aabb.min.x, vertex.position.x);
          aabb.min.y = std::min(aabb.min.y, vertex.position.y);
          aabb.min.z = std::min(aabb.min.z, vertex.position.z);
          aabb.max.x = std::max(aabb.max.x, vertex.position.x);
          aabb.max.y = std::max(aabb.max.y, vertex.position.y);
          aabb.max.z = std::max(aabb.max.z, vertex.position.z);
        }
        std::cout << "progress: " << i << "/" << aabb_count << "%\r";
        std::cout.flush();
        std::vector<float> aabb_data{
            aabb.min.x * transform->get_scale().x,
            aabb.min.y * transform->get_scale().y,
            aabb.min.z * transform->get_scale().z,
            0.0f,
            aabb.max.x * transform->get_scale().x,
            aabb.max.y * transform->get_scale().y,
            aabb.max.z * transform->get_scale().z,
        };
        Engine::get_module<Graphics>()->update_buffer(gizmo_constant_buffer,
                                                      aabb_data, i * stride);
      }

      // create render bundle
      auto color_format = wgpu::TextureFormat::BGRA8Unorm;
      wgpu::RenderBundleEncoderDescriptor render_bundle_encoder_desc{
          .label = "render bundle encoder",
          .colorFormatCount = 1,
          .colorFormats = &color_format,
          .depthStencilFormat = wgpu::TextureFormat::Depth24Plus,
          .sampleCount = 1,
          .depthReadOnly = false,
          .stencilReadOnly = true,
      };
      auto render_bundle_encoder =
          Engine::get_module<Graphics>()
              ->get_device()
              .CreateRenderBundleEncoder(&render_bundle_encoder_desc);

      render_bundle_encoder.SetPipeline(aabb_pass.render_pipeline);
      render_bundle_encoder.SetVertexBuffer(0, gizmo_vertex_buffer, 0,
                                            gizmo_vertex_buffer.GetSize());
      render_bundle_encoder.SetIndexBuffer(gizmo_index_buffer,
                                           wgpu::IndexFormat::Uint32, 0,
                                           gizmo_index_buffer.GetSize());
      render_bundle_encoder.SetBindGroup(1, gizmo_camera_bind_group, 0,
                                         nullptr);

      auto aabb_buffer_stride =
          Engine::get_module<Graphics>()->get_buffer_stride(sizeof(AABB));
      uint32_t offset = 0;
      for (int i = 0; i < aabb_count; i++) {
        offset = i * aabb_buffer_stride;
        render_bundle_encoder.SetBindGroup(0, gizmo_constant_bind_group, 1,
                                           &offset);
        render_bundle_encoder.DrawIndexed(
            gizmo_index_buffer.GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
      }
      aabb_render_bundle = render_bundle_encoder.Finish();
    }

    // update constants
    for (auto rendered : gpu_resources) {
      const auto entity =
          Engine::get_module<EntityStore>()->get(rendered.first);

      // update constant buffer
      auto& gpu_transform_buffer = rendered.second.transform_buffer;
      auto transform = entity->get_component<Transform>();
      const auto translate =
          Math::translation_matrix4x4f(transform->get_position());

      const auto scale = Math::scale_matrix4x4f(transform->get_scale());

      auto rotate =
          Math::euler_angle_to_matrix4x4f(transform->get_euler_angle());

      const auto world_mat = translate * rotate * scale;
      auto world_mat_vec = std::vector<float>();
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          world_mat_vec.push_back(world_mat.internal_data.at(j).at(i));
        }
      }

      Engine::get_module<Graphics>()->update_buffer(gpu_transform_buffer,
                                                    world_mat_vec);
    }

    auto rendered_entity_list = std::vector<EntityID>();
    for (const auto& entity : entity_list) {
      const auto& mesh = entity->get_component<Mesh>();
      const auto& transform = entity->get_component<Transform>();
      const auto& material = entity->get_component<Material>();

      auto entity_id = entity->get_id();
      if (mesh == nullptr || transform == nullptr) {
        dispose_gpu_resource(entity_id);
        continue;
      }

      if (!gpu_resources.contains(entity_id)) {
        MeshBuffer resource{};
        auto mesh_buffer = create_mesh_buffer(entity_id);
        resource.vertex_buffer = std::get<0>(mesh_buffer);
        resource.index_buffer = std::get<1>(mesh_buffer);
        resource.transform_buffer = create_constant_buffer(entity_id);

        gpu_resources.insert_or_assign(entity_id, resource);
      }

      if (!material->is_transparent) {
        rendered_entity_list.push_back(entity_id);
      }
    }
    render_bundle = create_diffuse_pass_render_bundle(rendered_entity_list);
  }

  // update camera
  {
    auto view_mat = Math::Matrix4x4f({{
        {1.0f, 0.0f, 0.0f, -camera_position.x},
        {0.0f, 1.0f, 0.0f, -camera_position.y},
        {0.0f, 0.0f, 1.0f, -camera_position.z},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    view_mat = Math::euler_angle_to_matrix4x4f(-camera_angle) * view_mat;

    auto ratio = 1080.0f / 1080.0f;
    auto focal_length = 2.0f;
    auto near = 0.01f;
    auto far = 100.0f;
    auto proj_mat = Math::perspective_projection(1.0f, ratio, near, far);
    auto scale = 10.0f;
    auto ortho_proj_mat =
        Math::orthographics_projection(scale, 1.0f, near, far);

    // auto view_proj_mat = ortho_proj_mat * view_mat;
    auto view_proj_mat = proj_mat * view_mat;

    auto view_proj_mat_vec = std::vector<float>();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        view_proj_mat_vec.push_back(view_proj_mat.internal_data.at(j).at(i));
      }
    }
    Engine::get_module<Graphics>()->update_buffer(camera_buffer,
                                                  view_proj_mat_vec);
  }

  auto command_encoder =
      Engine::get_module<Graphics>()->get_device().CreateCommandEncoder();

  // execute diffuse pass
  if (render_bundle != nullptr) {
    wgpu::RenderPassColorAttachment attachment{
        .view = render_target,
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {1.0f, 0.0f, 1.0f, 0.0f},
    };

    wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment{
        .view = diffuse_pass.depth_texture_view,
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f,
        .depthReadOnly = false,
        .stencilLoadOp = wgpu::LoadOp::Undefined,
        .stencilStoreOp = wgpu::StoreOp::Undefined,
        .stencilClearValue = 0,
        .stencilReadOnly = true,
    };

    wgpu::RenderPassDescriptor renderpass_desc{
        .colorAttachmentCount = 1,
        .colorAttachments = &attachment,
        .depthStencilAttachment = &depth_stencil_attachment};
    auto pass = command_encoder.BeginRenderPass(&renderpass_desc);
    pass.ExecuteBundles(1, &render_bundle);
    pass.End();
  }

  if (draw_aabb) {
    wgpu::RenderPassColorAttachment attachment{
        .view = render_target,
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {1.0f, 0.0f, 1.0f, 0.0f},
    };

    wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment{
        .view = diffuse_pass.depth_texture_view,
        .depthLoadOp = wgpu::LoadOp::Load,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f,
        .depthReadOnly = false,
        .stencilLoadOp = wgpu::LoadOp::Undefined,
        .stencilStoreOp = wgpu::StoreOp::Undefined,
        .stencilClearValue = 0,
        .stencilReadOnly = true,
    };

    wgpu::RenderPassDescriptor gizmo_pass_desc{
        .colorAttachmentCount = 1,
        .colorAttachments = &attachment,
        .depthStencilAttachment = &depth_stencil_attachment,
    };
    auto gizmo_pass = command_encoder.BeginRenderPass(&gizmo_pass_desc);
    gizmo_pass.ExecuteBundles(1, &aabb_render_bundle);
    gizmo_pass.End();
  }

  // execute texture pass
  {
    wgpu::RenderPassColorAttachment attachment{
        .view = render_target,
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {1.0f, 0.0f, 1.0f, 0.0f},
    };

    wgpu::RenderPassDescriptor texture_pass_desc{
        .colorAttachmentCount = 1,
        .colorAttachments = &attachment,
    };
    auto pass = command_encoder.BeginRenderPass(&texture_pass_desc);
    pass.SetPipeline(texture_pass.render_pipeline);
    pass.SetIndexBuffer(texture_pass_index_buffer, wgpu::IndexFormat::Uint32, 0,
                        texture_pass_index_buffer.GetSize());
    pass.SetBindGroup(0, texture_pass_test_bind_group, 0, nullptr);
    pass.Draw(6, 1, 0, 0);
    pass.End();
  }

  auto command_buffer = command_encoder.Finish();
  Engine::get_module<Graphics>()->get_device().GetQueue().Submit(
      1, &command_buffer);
}

std::tuple<wgpu::Buffer, wgpu::Buffer> ReflectionRenderer::create_mesh_buffer(
    EntityID id) {
  auto mesh = Engine::get_module<MeshStore>()->get(id);
  auto vertices = mesh->get_vertices();
  auto indices = mesh->get_indices();

  wgpu::Buffer vertex_buffer;
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "vertex buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc |
                 wgpu::BufferUsage::Vertex,
        .size = vertices.size() * sizeof(Vertex),
        .mappedAtCreation = false};

    vertex_buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(vertex_buffer, vertices);
  }

  wgpu::Buffer index_buffer;
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = indices.size() * sizeof(uint32_t),
        .mappedAtCreation = false};

    index_buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(index_buffer, indices);
  }
  return std::make_tuple(vertex_buffer, index_buffer);
}

wgpu::Buffer ReflectionRenderer::create_constant_buffer(EntityID entity_id) {
  auto transform = Engine::get_module<TransformStore>()->get(entity_id);
  auto position = transform->get_position();
  auto angle = transform->get_euler_angle();
  auto scale = transform->get_scale();

  wgpu::Buffer gpu_transform_buffer{};
  wgpu::BufferDescriptor buffer_desc{
      .nextInChain = nullptr,
      .label = "constant",
      .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
      .size = Engine::get_module<Graphics>()->get_buffer_stride(
          sizeof(Math::Matrix4x4f)),
      .mappedAtCreation = false,
  };

  auto buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);

  auto binding = wgpu::BindGroupEntry{.binding = 0,
                                      .buffer = buffer,
                                      .offset = 0,
                                      .size = sizeof(Math::Matrix4x4f)};
  wgpu::BindGroupDescriptor bind_group_desc{
      .layout = diffuse_pass.mesh_constant_bind_group_layout,
      .entryCount = 1,
      .entries = &binding};

  gpu_transform_buffer = buffer;

  return gpu_transform_buffer;
}

void ReflectionRenderer::dispose_gpu_resource(EntityID id) {
  if (gpu_resources.contains(id)) {
    auto& gpu_resource = gpu_resources.at(id);
    gpu_resource.vertex_buffer.Destroy();
    gpu_resource.index_buffer.Destroy();
    gpu_resource.transform_buffer.Destroy();
    gpu_resources.erase(id);
  }
}

bool ReflectionRenderer::load_texture(std::filesystem::path path) {
  if (textures.contains(path)) {
    return textures.at(path).is_transparent;
  }

  auto texture_data = DDSLoader::load(path);

  std::lock_guard<std::mutex> lock(texture_load_mutex);

  const wgpu::TextureDescriptor texture_desc{
      .nextInChain = nullptr,
      .usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding,
      .dimension = wgpu::TextureDimension::e2D,
      .size = {texture_data.width, texture_data.height, 1},
      .format = wgpu::TextureFormat::RGBA8Unorm,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
  };

  auto texture =
      Engine::get_module<Graphics>()->get_device().CreateTexture(&texture_desc);

  wgpu::ImageCopyTexture dest{
      .texture = texture,
      .mipLevel = 0,
      .origin = {0, 0, 0},
      .aspect = wgpu::TextureAspect::All,
  };

  wgpu::TextureDataLayout source{
      .offset = 0,
      .bytesPerRow = sizeof(DDSLoader::RGBA8888) * texture_data.width,
      .rowsPerImage = texture_data.height,
  };

  Engine::get_module<Graphics>()->get_device().GetQueue().WriteTexture(
      &dest, texture_data.data.data(), texture_data.data.size() * 4, &source,
      &texture_desc.size);

  auto texture_view_desc = wgpu::TextureViewDescriptor{
      .format = wgpu::TextureFormat::RGBA8Unorm,
      .dimension = wgpu::TextureViewDimension::e2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = wgpu::TextureAspect::All,
  };

  auto texture_view = texture.CreateView(&texture_view_desc);

  auto sampler_desc = wgpu::SamplerDescriptor{
      .addressModeU = wgpu::AddressMode::Repeat,
      .addressModeV = wgpu::AddressMode::Repeat,
      .addressModeW = wgpu::AddressMode::Repeat,
      .magFilter = wgpu::FilterMode::Linear,
      .minFilter = wgpu::FilterMode::Linear,
      .mipmapFilter = wgpu::MipmapFilterMode::Linear,
      .lodMinClamp = 0.0f,
      .lodMaxClamp = 0.0f,
      .compare = wgpu::CompareFunction::Undefined,
      .maxAnisotropy = 1,
  };
  sampler =
      Engine::get_module<Graphics>()->get_device().CreateSampler(&sampler_desc);

  GPUTexture gpu_texture{
      .texture = texture,
      .sampler = sampler,
      .texture_view = texture_view,
      .is_transparent = texture_data.alpha,
  };

  textures.insert_or_assign(path, gpu_texture);
  std::cout << "load texture: " << path << std::endl;
  return texture_data.alpha;
}

wgpu::RenderBundle ReflectionRenderer::create_diffuse_pass_render_bundle(
    std::vector<EntityID> render_list) {
  update_unified_mesh_buffer();

  // render bandle encoder
  auto color_format = wgpu::TextureFormat::BGRA8Unorm;
  wgpu::RenderBundleEncoderDescriptor render_bundle_encoder_desc{
      .label = "render bundle encoder",
      .colorFormatCount = 1,
      .colorFormats = &color_format,
      .depthStencilFormat = wgpu::TextureFormat::Depth24Plus,
      .sampleCount = 1,
      .depthReadOnly = false,
      .stencilReadOnly = true,
  };
  auto render_bundle_encoder =
      Engine::get_module<Graphics>()->get_device().CreateRenderBundleEncoder(
          &render_bundle_encoder_desc);

  // render
  {
    render_bundle_encoder.SetPipeline(diffuse_pass.render_pipeline);

    uint32_t instance_idx = 0;
    for (const auto& id : render_list) {
      auto mesh_buffer = gpu_resources.at(id);
      auto mat = textures.at(
          Engine::get_module<MaterialStore>()->get(id)->texture_path);

      // transform buffer
      auto binding = wgpu::BindGroupEntry{
          .binding = 0,
          .buffer = mesh_buffer.transform_buffer,
          .offset = 0,
          .size = sizeof(Math::Matrix4x4f),
      };
      auto transform_buffer_bind_group_desc = wgpu::BindGroupDescriptor{
          .layout = diffuse_pass.mesh_constant_bind_group_layout,
          .entryCount = 1,
          .entries = &binding,
      };
      auto transform_buffer_bind_group =
          Engine::get_module<Graphics>()->get_device().CreateBindGroup(
              &transform_buffer_bind_group_desc);

      // material buffer
      auto texture_bindings = std::vector<wgpu::BindGroupEntry>(2);
      texture_bindings.at(0) = wgpu::BindGroupEntry{
          .binding = 0,
          .textureView = mat.texture_view,
      };
      texture_bindings.at(1) = wgpu::BindGroupEntry{
          .binding = 1,
          .sampler = mat.sampler,
      };
      wgpu::BindGroupDescriptor texture_bind_group_desc{
          .layout = diffuse_pass.texture_bind_group_layout,
          .entryCount = static_cast<uint32_t>(texture_bindings.size()),
          .entries = texture_bindings.data(),
      };
      auto texture_bind_group =
          Engine::get_module<Graphics>()->get_device().CreateBindGroup(
              &texture_bind_group_desc);

      const auto& instance_data = instance_data_list.at(instance_idx++);

      render_bundle_encoder.SetVertexBuffer(
          0, mesh_buffer.vertex_buffer, 0, mesh_buffer.vertex_buffer.GetSize());
      render_bundle_encoder.SetIndexBuffer(mesh_buffer.index_buffer,
                                           wgpu::IndexFormat::Uint32, 0,
                                           mesh_buffer.index_buffer.GetSize());

      render_bundle_encoder.SetBindGroup(0, transform_buffer_bind_group, 0,
                                         nullptr);
      render_bundle_encoder.SetBindGroup(1, camera_constant_bind_group, 0,
                                         nullptr);
      render_bundle_encoder.SetBindGroup(2, texture_bind_group, 0, nullptr);
      render_bundle_encoder.DrawIndexed(
          mesh_buffer.index_buffer.GetSize() / sizeof(uint32_t), 1, 0, 0, 0);
    }
  }

  return render_bundle_encoder.Finish();
}

void ReflectionRenderer::update_unified_mesh_buffer() {
  if (unified_vertex_buffer != nullptr) {
    unified_vertex_buffer.Destroy();
  }
  if (unified_index_buffer != nullptr) {
    unified_index_buffer.Destroy();
  }

  instance_data_list.clear();
  instance_data_list.reserve(gpu_resources.size());

  // count size
  uint64_t vertex_offset = 0;
  uint64_t index_offset = 0;
  for (const auto& gpu_resource : gpu_resources) {
    const auto id = gpu_resource.first;
    const auto vertex_buffer_size =
        Engine::get_module<MeshStore>()->get(id)->get_vertices().size() *
        sizeof(Vertex);

    const auto index_buffer_size =
        Engine::get_module<MeshStore>()->get(id)->get_indices().size() *
        sizeof(uint32_t);

    instance_data_list.push_back(InstanceData{.vertex_offset = vertex_offset,
                                              .index_offset = index_offset,
                                              .vertex_size = vertex_buffer_size,
                                              .index_size = index_buffer_size});

    vertex_offset += vertex_buffer_size;
    index_offset += index_buffer_size;

    std::cout << "offset: " << vertex_offset << ", " << index_offset
              << std::endl;
  }

  // allocate buffer
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "unified vertex buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
        .size = vertex_offset,
        .mappedAtCreation = false};

    unified_vertex_buffer =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
  }

  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "unified index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = index_offset,
        .mappedAtCreation = false};

    unified_index_buffer =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
  }

  // update buffer
  for (const auto& gpu_resource : gpu_resources) {
    const auto entity_id = gpu_resource.first;

    const auto& instance_data = instance_data_list.at(entity_id);

    const auto& vertex_data =
        Engine::get_module<MeshStore>()->get(entity_id)->get_vertices();
    const auto& index_data =
        Engine::get_module<MeshStore>()->get(entity_id)->get_indices();
    Engine::get_module<Graphics>()->update_buffer(
        unified_vertex_buffer, vertex_data, instance_data.vertex_offset);
    Engine::get_module<Graphics>()->update_buffer(
        unified_index_buffer, index_data, instance_data.index_offset);
  }
}