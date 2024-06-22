#include "screenspace_mesh_renderer.hpp"

#include "engine.hpp"
#include "graphics.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "test_image.h"

using namespace ShiftFlamework;

void ScreenSpaceMeshRenderer::initialize() {
  std::vector<wgpu::VertexAttribute> vertex_attributes(2);
  vertex_attributes.at(0).format = wgpu::VertexFormat::Float32x2;
  vertex_attributes.at(0).offset = 0;
  vertex_attributes.at(0).shaderLocation = 0;

  vertex_attributes.at(1).format = wgpu::VertexFormat::Float32x2;
  vertex_attributes.at(1).offset = 2 * 4;  // 2 * sizeof(float32)
  vertex_attributes.at(1).shaderLocation = 1;

  wgpu::VertexBufferLayout vertex_buffer_layout{
      .arrayStride = (2 + 2) * 4,  //(2 + 2) * sizeof(float32)
      .stepMode = wgpu::VertexStepMode::Vertex,
      .attributeCount = static_cast<uint32_t>(vertex_attributes.size()),
      .attributes = vertex_attributes.data(),
  };

  wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
  wgsl_desc.code = R"(
      @group(0) @binding(0) var<uniform> world_mat: mat4x4f;
      @group(1) @binding(0) var gradientTexture: texture_2d<f32>;
      @group(1) @binding(1) var textureSampler: sampler;
      @group(1) @binding(2) var<uniform> texture_offset: vec2f;
      @group(1) @binding(3) var<uniform> tile_scale: vec2f;

      struct VertexInput{
        @location(0) position: vec2f,
        @location(1) texture_coord: vec2f,
      }

      struct VertexOutput{
        @builtin(position) position: vec4f,
        @location(0) texture_coord: vec2f,
      }

      @vertex fn vertexMain(in: VertexInput) -> VertexOutput {
        var out: VertexOutput;
        var p: vec4f;
        p = world_mat * vec4f(in.position, 0.0, 1.0);
        p /= p.w;
        out.position = p;
        out.texture_coord = in.texture_coord;
        return out;
      }

      @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f {
        return vec4f(textureSample(gradientTexture, textureSampler, in.texture_coord * tile_scale + texture_offset).rgb, 1.0);
      }
    )";
  wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
  wgpu::ShaderModule shader_module =
      Engine::get_module<Graphics>()->get_device().CreateShaderModule(
          &shader_module_desc);

  wgpu::ColorTargetState color_target_state{
      .format = wgpu::TextureFormat::BGRA8Unorm};

  wgpu::FragmentState fragment_state{
      .module = shader_module,
      .entryPoint = "fragmentMain",
      .targetCount = 1,
      .targets = &color_target_state,
  };

  auto binding_layout_entries = std::vector<wgpu::BindGroupLayoutEntry>(5);
  // constant
  binding_layout_entries.at(0) =
      wgpu::BindGroupLayoutEntry{.binding = 0,
                                 .visibility = wgpu::ShaderStage::Vertex,
                                 .buffer = wgpu::BufferBindingLayout{
                                     .type = wgpu::BufferBindingType::Uniform,
                                     .hasDynamicOffset = false,
                                     .minBindingSize = sizeof(Math::Matrix4x4f),
                                 }};

  // texture
  binding_layout_entries.at(1) = wgpu::BindGroupLayoutEntry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Fragment,
      .texture =
          wgpu::TextureBindingLayout{
              .sampleType = wgpu::TextureSampleType::Float,
              .viewDimension = wgpu::TextureViewDimension::e2D},
  };
  // sampler
  binding_layout_entries.at(2) = wgpu::BindGroupLayoutEntry{
      .binding = 1,
      .visibility = wgpu::ShaderStage::Fragment,
      .sampler = wgpu::SamplerBindingLayout{
          .type = wgpu::SamplerBindingType::Filtering}};
  // offset
  binding_layout_entries.at(3) = wgpu::BindGroupLayoutEntry{
      .binding = 2,
      .visibility = wgpu::ShaderStage::Fragment,
      .buffer =
          wgpu::BufferBindingLayout{.type = wgpu::BufferBindingType::Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(Math::Vector2f)},
  };
  // tile scale
  binding_layout_entries.at(4) = wgpu::BindGroupLayoutEntry{
      .binding = 3,
      .visibility = wgpu::ShaderStage::Fragment,
      .buffer =
          wgpu::BufferBindingLayout{.type = wgpu::BufferBindingType::Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(Math::Vector2f)},
  };

  std::vector<wgpu::BindGroupLayoutEntry> constant_layout_entries(
      binding_layout_entries.begin(), binding_layout_entries.begin() + 1);
  wgpu::BindGroupLayoutDescriptor constant_bind_group_layout_desc = {
      .entryCount = 1, .entries = constant_layout_entries.data()};
  constant_bind_group_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &constant_bind_group_layout_desc);

  std::vector<wgpu::BindGroupLayoutEntry> texture_layout_entries(
      binding_layout_entries.begin() + 1, binding_layout_entries.end());
  wgpu::BindGroupLayoutDescriptor texture_bind_group_layout_desc = {
      .entryCount = 4,
      .entries = texture_layout_entries.data(),
  };

  texture_bind_group_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &texture_bind_group_layout_desc);
  std::vector<wgpu::BindGroupLayout> layouts = {constant_bind_group_layout,
                                                texture_bind_group_layout};
  wgpu::PipelineLayoutDescriptor layout_desc{
      .bindGroupLayoutCount = 2, .bindGroupLayouts = layouts.data()};

  wgpu::PipelineLayout pipeline_layout =
      Engine::get_module<Graphics>()->get_device().CreatePipelineLayout(
          &layout_desc);

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .layout = pipeline_layout,
      .vertex = {.module = shader_module,
                 .entryPoint = "vertexMain",
                 .bufferCount = 1,
                 .buffers = &vertex_buffer_layout},
      .fragment = &fragment_state};

  render_pipeline =
      Engine::get_module<Graphics>()->get_device().CreateRenderPipeline(
          &render_pipeline_desc);
}

void ScreenSpaceMeshRenderer::render(wgpu::TextureView render_target) {
  // update constant
  for (const auto& mesh : mesh_list) {
    if (const auto& transform =
            mesh->get_entity()->get_component<ScreenSpaceTransform>()) {
      auto buffer_exists =
          gpu_transform_buffers.contains(mesh->get_entity()->get_id());

      if (!buffer_exists) {
        wgpu::BufferDescriptor buffer_desc{
            .nextInChain = nullptr,
            .label = "constant",
            .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
            .size = Engine::get_module<Graphics>()->get_buffer_stride(
                sizeof(Math::Matrix4x4f)),
            .mappedAtCreation = false,
        };

        auto constant_buffer =
            Engine::get_module<Graphics>()->create_buffer(buffer_desc);

        auto bindgroup = create_constant_bind_group(constant_buffer);

        auto gpu_transform_buffer = std::make_shared<GPUTransformBuffer>();
        gpu_transform_buffer->buffer = constant_buffer;
        gpu_transform_buffer->bindgroup = bindgroup;

        gpu_transform_buffers.insert_or_assign(mesh->get_entity()->get_id(),
                                               gpu_transform_buffer);
      }

      auto gpu_transform_buffer =
          gpu_transform_buffers.at(mesh->get_entity()->get_id());
      // update constant buffer
      const auto rotate_mat =
          Math::Matrix4x4f({{{std::cos(transform->get_angle()),
                              -std::sin(transform->get_angle()), 0, 0},
                             {std::sin(transform->get_angle()),
                              std::cos(transform->get_angle()), 0, 0},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1}}});
      const auto translate_mat =
          Math::Matrix4x4f({{{1, 0, 0, transform->get_position().x},
                             {0, 1, 0, transform->get_position().y},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1}}});

      const auto scale_mat =
          Math::Matrix4x4f({{{transform->get_scale().x, 0, 0, 0},
                             {0, transform->get_scale().y, 0, 0},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1}}});

      const auto matrix = translate_mat * rotate_mat * scale_mat;

      Engine::get_module<Graphics>()->update_buffer(
          gpu_transform_buffer->buffer, std::vector(1, transposed(matrix)));
    }

    // render
    wgpu::RenderPassColorAttachment attachment{.view = render_target,
                                               .loadOp = wgpu::LoadOp::Clear,
                                               .storeOp = wgpu::StoreOp::Store};

    wgpu::RenderPassDescriptor renderpass_desc{.colorAttachmentCount = 1,
                                               .colorAttachments = &attachment};

    wgpu::CommandEncoder encoder =
        Engine::get_module<Graphics>()->get_device().CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass_desc);
    pass.SetPipeline(render_pipeline);

    for (const auto& mesh : mesh_list) {
      auto entity_id = mesh->get_entity()->get_id();
      auto gpu_buffer = gpu_mesh_buffers.at(entity_id);
      pass.SetVertexBuffer(
          0, gpu_buffer->vertex_buffer, 0,
          mesh->get_vertices().size() * sizeof(ScreenSpaceVertex));

      pass.SetIndexBuffer(gpu_buffer->index_buffer, wgpu::IndexFormat::Uint32,
                          0, mesh->get_indices().size() * sizeof(uint32_t));

      auto transform_buffer = gpu_transform_buffers.at(entity_id);
      pass.SetBindGroup(0, transform_buffer->bindgroup, 0, nullptr);
      pass.SetBindGroup(
          1, mesh->get_entity()->get_component<Material>()->get_bindgroup(), 0,
          nullptr);
      pass.DrawIndexed(mesh->get_indices().size(), 1, 0, 0, 0);
    }
    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    Engine::get_module<Graphics>()->get_device().GetQueue().Submit(1,
                                                                   &commands);
  }
}

wgpu::BindGroup ScreenSpaceMeshRenderer::create_constant_bind_group(
    const wgpu::Buffer& constant_buffer) {
  auto constant_binding =
      wgpu::BindGroupEntry{.binding = 0,
                           .buffer = constant_buffer,
                           .offset = 0,
                           .size = sizeof(Math::Matrix4x4f)};

  wgpu::BindGroupDescriptor bind_group_desc{
      .layout = constant_bind_group_layout,
      .entryCount = 1,
      .entries = &constant_binding};

  return Engine::get_module<Graphics>()->get_device().CreateBindGroup(
      &bind_group_desc);
}

wgpu::BindGroup ScreenSpaceMeshRenderer::create_texture_bind_group(
    const wgpu::TextureView& texture_view, const wgpu::Sampler& sampler,
    const wgpu::Buffer& tex_offset, const wgpu::Buffer& tile_scale) {
  auto texture_bindings = std::vector<wgpu::BindGroupEntry>(4);
  texture_bindings.at(0) = wgpu::BindGroupEntry{
      .binding = 0,
      .textureView = texture_view,
  };
  texture_bindings.at(1) = wgpu::BindGroupEntry{
      .binding = 1,
      .sampler = sampler,
  };
  texture_bindings.at(2) = wgpu::BindGroupEntry{
      .binding = 2,
      .buffer = tex_offset,
      .offset = 0,
      .size = sizeof(Math::Vector2f),
  };
  texture_bindings.at(3) = wgpu::BindGroupEntry{
      .binding = 3,
      .buffer = tile_scale,
      .offset = 0,
      .size = sizeof(Math::Vector2f),
  };

  wgpu::BindGroupDescriptor texture_bind_group_desc{
      .layout = texture_bind_group_layout,
      .entryCount = 4,
      .entries = texture_bindings.data(),
  };
  return Engine::get_module<Graphics>()->get_device().CreateBindGroup(
      &texture_bind_group_desc);
}

void ScreenSpaceMeshRenderer::register_mesh(
    std::shared_ptr<ScreenSpaceMesh> mesh_component) {
  mesh_list.push_back(mesh_component);

  auto gpu_resource = std::make_shared<GPUMeshBuffer>();
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "vertex buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
        .size =
            mesh_component->get_vertices().size() * sizeof(ScreenSpaceVertex),
        .mappedAtCreation = false};

    gpu_resource->vertex_buffer =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(
        gpu_resource->vertex_buffer, mesh_component->get_vertices());
  }
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = mesh_component->get_indices().size() * sizeof(uint32_t),
        .mappedAtCreation = false,
    };
    gpu_resource->index_buffer =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(
        gpu_resource->index_buffer, mesh_component->get_indices());
  }
  gpu_mesh_buffers.insert_or_assign(mesh_component->get_entity()->get_id(),
                                    gpu_resource);
}

void ScreenSpaceMeshRenderer::unregister_mesh(
    std::shared_ptr<ScreenSpaceMesh> mesh_component) {
  // todo
  // if we have id for each component(entity)
  // we can use hash map and finish this operation O(1)
  auto ptr = std::begin(mesh_list);
  while (ptr->get() != mesh_component.get() && ptr != std::end(mesh_list))
    ptr++;

  auto removed_mesh_id = mesh_component->get_entity()->get_id();
  mesh_list.erase(ptr);

  auto removed_mesh_gpu_buffer = gpu_mesh_buffers.at(removed_mesh_id);
  removed_mesh_gpu_buffer->vertex_buffer.Destroy();
  removed_mesh_gpu_buffer->index_buffer.Destroy();
  gpu_mesh_buffers.erase(removed_mesh_id);
}