#include "reflection_renderer.hpp"

#include <stdint.h>

#include <initializer_list>

#include "dds_loader.hpp"
#include "engine.hpp"
#include "entity.hpp"
#include "gpu_mesh_buffer.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include "vector.hpp"

using namespace SF;

void ReflectionRenderer::initialize() {
  std::vector<wgpu::VertexAttribute> vertex_attributes(4);
  // position
  vertex_attributes.at(0).format = wgpu::VertexFormat::Float32x4;
  vertex_attributes.at(0).offset = 0;
  vertex_attributes.at(0).shaderLocation = 0;
  // normal
  vertex_attributes.at(1).format = wgpu::VertexFormat::Float32x3;
  vertex_attributes.at(1).offset = 4 * 4;  // 4 * sizeof(Float32)
  vertex_attributes.at(1).shaderLocation = 1;
  // tangent
  vertex_attributes.at(2).format = wgpu::VertexFormat::Float32x4;
  vertex_attributes.at(2).offset = 7 * 4;  // (4 + 3)  * sizeof(Float32)
  vertex_attributes.at(2).shaderLocation = 2;
  // texcoord
  vertex_attributes.at(3).format = wgpu::VertexFormat::Float32x2;
  vertex_attributes.at(3).offset = 11 * 4;  // (4 + 3 + 4) * sizeof(Float32)
  vertex_attributes.at(3).shaderLocation = 3;

  wgpu::VertexBufferLayout vertex_buffer_layout{
      .arrayStride = 13 * 4,  // (4 + 3 + 4 + 2) * sizeof(Float32)
      .stepMode = wgpu::VertexStepMode::Vertex,
      .attributeCount = static_cast<uint32_t>(vertex_attributes.size()),
      .attributes = vertex_attributes.data()};

  wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
  wgsl_desc.code = R"(
    @group(0) @binding(0) var<uniform> world_mat: mat4x4f;
    @group(1) @binding(0) var<uniform> view_proj_mat: mat4x4f;
    @group(2) @binding(0) var tex: texture_2d<f32>;
    @group(2) @binding(1) var tex_sampler: sampler;

    struct VertexInput{
      @location(0) position: vec4f,
      @location(1) normal: vec3f,
      @location(2) tangent: vec4f,
      @location(3) texcoord0: vec2f,
    };

    struct VertexOutput{
        @builtin(position) position: vec4f,
        @location(0) texcoord0: vec2f,
        @location(1) normal: vec3f,
    }

    @vertex fn vertexMain(in: VertexInput) -> VertexOutput{
        var out: VertexOutput;
        out.position = view_proj_mat * world_mat * in.position;
        // note that normalizing w is automatically done by the hardware
        // see https://eliemichel.github.io/LearnWebGPU/basic-3d-rendering/3d-meshes/projection-matrices.html#perspective-matrix

        out.texcoord0 = in.texcoord0;
        out.normal = in.normal;
        return out;
    }

    @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f{
        // return vec4f(in.texcoord0, 0.0, 1.0);
        return vec4f(textureSample(tex, tex_sampler, in.texcoord0).xyz, 1.0);
    }
  )";

  wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
  wgpu::ShaderModule shader_module =
      Engine::get_module<Graphics>()->get_device().CreateShaderModule(
          &shader_module_desc);

  wgpu::ColorTargetState color_target_state{
      .format = wgpu::TextureFormat::BGRA8Unorm,
  };

  wgpu::FragmentState fragment_state{
      .module = shader_module,
      .entryPoint = "fragmentMain",
      .targetCount = 1,
      .targets = &color_target_state,
  };

  auto binding_layout_entries = std::vector<wgpu::BindGroupLayoutEntry>(4);
  // mesh constant
  binding_layout_entries.at(0) =
      wgpu::BindGroupLayoutEntry{.binding = 0,
                                 .visibility = wgpu::ShaderStage::Vertex,
                                 .buffer = wgpu::BufferBindingLayout{
                                     .type = wgpu::BufferBindingType::Uniform,
                                     .hasDynamicOffset = false,
                                     .minBindingSize = sizeof(Math::Matrix4x4f),
                                 }};

  // camera constant
  binding_layout_entries.at(1) =
      wgpu::BindGroupLayoutEntry{.binding = 0,
                                 .visibility = wgpu::ShaderStage::Vertex,
                                 .buffer = wgpu::BufferBindingLayout{
                                     .type = wgpu::BufferBindingType::Uniform,
                                     .hasDynamicOffset = false,
                                     .minBindingSize = sizeof(Math::Matrix4x4f),
                                 }};

  // texture
  binding_layout_entries.at(2) = wgpu::BindGroupLayoutEntry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Fragment,
      .texture =
          wgpu::TextureBindingLayout{
              .sampleType = wgpu::TextureSampleType::Float,
              .viewDimension = wgpu::TextureViewDimension::e2D},
  };

  // sampler
  binding_layout_entries.at(3) = wgpu::BindGroupLayoutEntry{
      .binding = 1,
      .visibility = wgpu::ShaderStage::Fragment,
      .sampler =
          wgpu::SamplerBindingLayout{.type =
                                         wgpu::SamplerBindingType::Filtering},
  };

  std::vector<wgpu::BindGroupLayoutEntry> mesh_constant_layout_entries(
      binding_layout_entries.begin(), binding_layout_entries.begin() + 1);
  wgpu::BindGroupLayoutDescriptor mesh_constant_layout_desc = {
      .entryCount = static_cast<uint32_t>(mesh_constant_layout_entries.size()),
      .entries = mesh_constant_layout_entries.data(),
  };
  mesh_constant_bind_group_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &mesh_constant_layout_desc);

  std::vector<wgpu::BindGroupLayoutEntry> camera_layout_entries(
      binding_layout_entries.begin() + 1, binding_layout_entries.begin() + 2);
  wgpu::BindGroupLayoutDescriptor camera_layout_desc = {
      .entryCount = static_cast<uint32_t>(camera_layout_entries.size()),
      .entries = camera_layout_entries.data(),
  };
  camera_constant_bind_group_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &camera_layout_desc);

  std::vector<wgpu::BindGroupLayoutEntry> texture_layout_entries(
      binding_layout_entries.begin() + 2, binding_layout_entries.begin() + 4);
  wgpu::BindGroupLayoutDescriptor texture_layout_desc = {
      .entryCount = static_cast<uint32_t>(texture_layout_entries.size()),
      .entries = texture_layout_entries.data(),
  };
  texture_bind_group_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &texture_layout_desc);

  std::vector<wgpu::BindGroupLayout> layouts = {
      mesh_constant_bind_group_layout, camera_constant_bind_group_layout,
      texture_bind_group_layout};
  wgpu::PipelineLayoutDescriptor layout_desc{
      .bindGroupLayoutCount = static_cast<uint32_t>(layouts.size()),
      .bindGroupLayouts = layouts.data(),
  };

  wgpu::PipelineLayout pipeline_layout =
      Engine::get_module<Graphics>()->get_device().CreatePipelineLayout(
          &layout_desc);

  wgpu::DepthStencilState depth_stencil_state{
      .nextInChain = nullptr,
      .format = wgpu::TextureFormat::Depth24Plus,
      .depthWriteEnabled = true,
      .depthCompare = wgpu::CompareFunction::Less,
      .stencilFront =
          wgpu::StencilFaceState{.compare = wgpu::CompareFunction::Always,
                                 .failOp = wgpu::StencilOperation::Keep,
                                 .depthFailOp = wgpu::StencilOperation::Keep,
                                 .passOp = wgpu::StencilOperation::Keep},
      .stencilBack =
          wgpu::StencilFaceState{.compare = wgpu::CompareFunction::Always,
                                 .failOp = wgpu::StencilOperation::Keep,
                                 .depthFailOp = wgpu::StencilOperation::Keep,
                                 .passOp = wgpu::StencilOperation::Keep},
      .stencilReadMask = 0,
      .stencilWriteMask = 0,
      .depthBias = 0,
      .depthBiasSlopeScale = 0.0f,
      .depthBiasClamp = 0.0f,
  };

  wgpu::TextureDescriptor depthTextureDesc{
      .nextInChain = nullptr,
      .usage = wgpu::TextureUsage::RenderAttachment,
      .dimension = wgpu::TextureDimension::e2D,
      .size = {1080, 1080, 1},
      .format = wgpu::TextureFormat::Depth24Plus,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .viewFormatCount = 1,
      .viewFormats = &depth_stencil_state.format,
  };

  wgpu::Texture depthTexture =
      Engine::get_module<Graphics>()->get_device().CreateTexture(
          &depthTextureDesc);

  wgpu::TextureViewDescriptor depthTextureViewDesc{
      .nextInChain = nullptr,
      .format = wgpu::TextureFormat::Depth24Plus,
      .dimension = wgpu::TextureViewDimension::e2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = wgpu::TextureAspect::DepthOnly,
  };

  depthTextureView = depthTexture.CreateView(&depthTextureViewDesc);

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .layout = pipeline_layout,
      .vertex = {.module = shader_module,
                 .entryPoint = "vertexMain",
                 .bufferCount = 1,
                 .buffers = &vertex_buffer_layout},
      .depthStencil = &depth_stencil_state,
      .fragment = &fragment_state,
  };

  render_pipeline =
      Engine::get_module<Graphics>()->get_device().CreateRenderPipeline(
          &render_pipeline_desc);

  // set up sample data
  {
    auto ratio = 1080.0f / 1080.0f;
    auto focal_length = 2.0f;
    auto near = 0.01f;
    auto far = 100.0f;
    auto divides = 1.0f / (focal_length * (far - near));
    auto view_proj_mat = Math::Matrix4x4f({{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, ratio, 0.0f, 0.0f},
        {0.0f, 0.0f, far * divides, -far * near * divides},
        {0.0f, 0.0f, 1.0f / focal_length, 1.0f},
    }});
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
        .size = sizeof(Math::Matrix4x4f),
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
        .layout = camera_constant_bind_group_layout,
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
        .layout = texture_bind_group_layout,
        .entryCount = static_cast<uint32_t>(texture_bindings.size()),
        .entries = texture_bindings.data(),
    };

    texture_bind_group =
        Engine::get_module<Graphics>()->get_device().CreateBindGroup(
            &texture_bind_group_desc);
  }
}

void ReflectionRenderer::render(wgpu::TextureView render_target) {
  // update constants
  auto entity_list = Engine::get_module<EntityStore>()->get_all();

  // create gpu resources
  for (const auto& entity : entity_list) {
    const auto& mesh = entity->get_component<Mesh>();
    const auto& transform = entity->get_component<Transform>();

    auto entity_id = entity->get_id();
    if (mesh == nullptr || transform == nullptr) {
      dispose_gpu_resource(entity_id);
      continue;
    }

    if (!gpu_resources.contains(entity_id)) {
      GPUResource resource{};
      resource.mesh_buffer = create_mesh_buffer(entity_id);
      resource.transform_buffer = create_constant_buffer(entity_id);

      gpu_resources.insert_or_assign(entity_id, resource);
    }
  }

  // update constants
  for (auto rendered : gpu_resources) {
    const auto entity = Engine::get_module<EntityStore>()->get(rendered.first);

    // update constant buffer
    auto& gpu_transform_buffer = rendered.second.transform_buffer;
    auto transform = entity->get_component<Transform>();
    const auto translate = Math::Matrix4x4f({{
        {1.0f, 0.0f, 0.0f, transform->get_position().x},
        {0.0f, 1.0f, 0.0f, transform->get_position().y},
        {0.0f, 0.0f, 1.0f, transform->get_position().z},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    const auto scale = Math::Matrix4x4f({{
        {transform->get_scale().x, 0.0f, 0.0f, 0.0f},
        {0.0f, transform->get_scale().y, 0.0f, 0.0f},
        {0.0f, 0.0f, transform->get_scale().z, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    const auto ax = transform->get_euler_angle().x;
    const auto ay = transform->get_euler_angle().y;
    const auto az = transform->get_euler_angle().z;
    const auto rotate_x = Math::Matrix4x4f({{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, cos(ax), -sin(ax), 0.0f},
        {0.0f, sin(ax), cos(ax), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    const auto rotate_y = Math::Matrix4x4f({{
        {cos(ay), 0.0f, sin(ay), 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {-sin(ay), 0.0f, cos(ay), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    const auto rotate_z = Math::Matrix4x4f({{
        {cos(az), -sin(az), 0.0f, 0.0f},
        {sin(az), cos(az), 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    const auto world_mat = translate * rotate_z * rotate_y * rotate_x * scale;
    auto world_mat_vec = std::vector<float>();
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        world_mat_vec.push_back(world_mat.internal_data.at(j).at(i));
      }
    }

    Engine::get_module<Graphics>()->update_buffer(gpu_transform_buffer.buffer,
                                                  world_mat_vec);
  }

  // update camera
  {
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::A) ==
        ButtonState::HOLD) {
      camera_position.x -= 0.1f;
    }
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::D) ==
        ButtonState::HOLD) {
      camera_position.x += 0.1f;
    }
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::W) ==
        ButtonState::HOLD) {
      camera_position.z += 0.1f;
    }
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::S) ==
        ButtonState::HOLD) {
      camera_position.z -= 0.1f;
    }
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::Q) ==
		ButtonState::HOLD) {
		camera_position.y += 0.1f;
	}
    	if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::E) == ButtonState::HOLD) {
camera_position.y -= 0.1f;
}


    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::H) ==
        ButtonState::HOLD) {
        camera_angle.y += 0.1f;
    }
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::L) ==
		ButtonState::HOLD) {
		camera_angle.y -= 0.1f;
	}
	if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::J) ==
		ButtonState::HOLD) {
		camera_angle.x += 0.1f;
	}
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::K) == ButtonState::HOLD) {
        camera_angle.x -= 0.1f;
    }

    auto view_mat = Math::Matrix4x4f({{
        {1.0f, 0.0f, 0.0f, -camera_position.x},
        {0.0f, 1.0f, 0.0f, -camera_position.y},
        {0.0f, 0.0f, 1.0f, -camera_position.z},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

    auto ratio = 1080.0f / 1080.0f;
    auto focal_length = 2.0f;
    auto near = 0.01f;
    auto far = 100.0f;
    auto divides = 1.0f / (focal_length * (far - near));
    auto proj_mat = Math::Matrix4x4f({{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, ratio, 0.0f, 0.0f},
        {0.0f, 0.0f, far * divides, -far * near * divides},
        {0.0f, 0.0f, 1.0f / focal_length, 1.0f},
    }});
    auto scale = 10.0f;
    auto ortho_proj_mat = Math::Matrix4x4f({{
        {1.0f / scale, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f / scale, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f / (far - near), -near / (far - near)},
        {0.0f, 0.0f, 0.0f, 1.0f},
    }});

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

  // render
  wgpu::RenderPassColorAttachment attachment{
      .view = render_target,
      .loadOp = wgpu::LoadOp::Clear,
      .storeOp = wgpu::StoreOp::Store,
  };

  wgpu::RenderPassDepthStencilAttachment depth_stencil_attachment{
      .view = depthTextureView,
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

  wgpu::CommandEncoder encoder =
      Engine::get_module<Graphics>()->get_device().CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass_desc);
  pass.SetPipeline(render_pipeline);

  for (const auto& rendered : gpu_resources) {
    // std::cout << "rendered: " << rendered.first << " #indice: "
    //           << rendered.second.mesh_buffer.index_buffer.GetSize() /
    //                  sizeof(uint32_t)
    //           << std::endl;
    auto id = rendered.first;
    auto mat_id = Engine::get_module<MaterialStore>()->get(id)->id;
    if (!textures.contains(mat_id)) {
      continue;
    }
    pass.SetVertexBuffer(0, rendered.second.mesh_buffer.vertex_buffer, 0,
                         rendered.second.mesh_buffer.vertex_buffer.GetSize());
    pass.SetIndexBuffer(rendered.second.mesh_buffer.index_buffer,
                        wgpu::IndexFormat::Uint32, 0,
                        rendered.second.mesh_buffer.index_buffer.GetSize());
    pass.SetBindGroup(0, rendered.second.transform_buffer.bindgroup, 0,
                      nullptr);
    pass.SetBindGroup(1, camera_constant_bind_group, 0, nullptr);
    pass.SetBindGroup(2, textures[mat_id].bind_group, 0, nullptr);
    pass.DrawIndexed(
        rendered.second.mesh_buffer.index_buffer.GetSize() / sizeof(uint32_t),
        1, 0, 0, 0);
  }

  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();

  Engine::get_module<Graphics>()->get_device().GetQueue().Submit(1, &commands);
}

GPUMeshBuffer ReflectionRenderer::create_mesh_buffer(EntityID id) {
  auto mesh = Engine::get_module<MeshStore>()->get(id);
  auto vertices = mesh->get_vertices();
  auto indices = mesh->get_indices();

  GPUMeshBuffer gpu_mesh_buffer{};
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "vertex buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
        .size = vertices.size() * sizeof(Vertex),
        .mappedAtCreation = false};

    gpu_mesh_buffer.vertex_buffer =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(gpu_mesh_buffer.vertex_buffer,
                                                  vertices);
  }

  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = indices.size() * sizeof(uint32_t),
        .mappedAtCreation = false};

    gpu_mesh_buffer.index_buffer =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(gpu_mesh_buffer.index_buffer,
                                                  indices);
  }
  return gpu_mesh_buffer;
}

GPUTransformBuffer ReflectionRenderer::create_constant_buffer(
    EntityID entity_id) {
  auto transform = Engine::get_module<TransformStore>()->get(entity_id);
  auto position = transform->get_position();
  auto angle = transform->get_euler_angle();
  auto scale = transform->get_scale();

  GPUTransformBuffer gpu_transform_buffer{};
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
      .layout = mesh_constant_bind_group_layout,
      .entryCount = 1,
      .entries = &binding};

  gpu_transform_buffer.buffer = buffer;
  gpu_transform_buffer.bindgroup =
      Engine::get_module<Graphics>()->get_device().CreateBindGroup(
          &bind_group_desc);

  return gpu_transform_buffer;
}

void ReflectionRenderer::dispose_gpu_resource(EntityID id) {
  if (gpu_resources.contains(id)) {
    auto& gpu_resource = gpu_resources.at(id);
    gpu_resource.mesh_buffer.vertex_buffer.Destroy();
    gpu_resource.mesh_buffer.index_buffer.Destroy();
    gpu_resource.transform_buffer.buffer.Destroy();
    gpu_resources.erase(id);
  }
}

void ReflectionRenderer::load_texture(std::string name, std::string path) {
  if (textures.contains(name)) {
    return;
  }
  auto texture_data = DDSLoader::load(path);

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
      .layout = texture_bind_group_layout,
      .entryCount = static_cast<uint32_t>(texture_bindings.size()),
      .entries = texture_bindings.data(),
  };

  auto texture_bind_group =
      Engine::get_module<Graphics>()->get_device().CreateBindGroup(
          &texture_bind_group_desc);

  GPUTexture gpu_texture{
      .texture = texture,
      .sampler = sampler,
      .texture_view = texture_view,
      .bind_group = texture_bind_group,
  };

  textures.insert_or_assign(name, gpu_texture);
}