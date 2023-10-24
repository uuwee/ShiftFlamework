#include "screenspace_mesh_renderer.hpp"

#include "engine.hpp"
#include "graphics.hpp"
#include "matrix.hpp"

using namespace ShiftFlamework;

uint32_t ceil_to_next_multiple(uint32_t value, uint32_t step) {
  uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
  return step * divide_and_ceil;
}

void ScreenSpaceMeshRenderer::initialize(uint32_t max_mesh_count) {
  max_instance_count = max_mesh_count;

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

      struct VertexInput{
        @location(0) position: vec2f,
        @location(1) texture_coord: vec2f,
      }

      struct VertexOutput{
        @builtin(position) position: vec4f,
        @location(0) color: vec3f,
      }

      @vertex fn vertexMain(in: VertexInput) -> VertexOutput {
        var out: VertexOutput;
        var p: vec4f;
        p = world_mat * vec4f(in.position, 0.0, 1.0);
        p /= p.w;
        out.position = p;
        out.color = vec3f(in.texture_coord, 0); 
        return out;
      }

      @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f {
        return vec4f(in.color, 1.0);
      }
    )";
  wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
  wgpu::ShaderModule shader_module =
      Engine::get_module<Graphics>()->device.CreateShaderModule(
          &shader_module_desc);

  wgpu::ColorTargetState color_target_state{
      .format = wgpu::TextureFormat::BGRA8Unorm};

  wgpu::FragmentState fragment_state{
      .module = shader_module,
      .entryPoint = "fragmentMain",
      .targetCount = 1,
      .targets = &color_target_state,
  };

  wgpu::BindGroupLayoutEntry binding_layout_entry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Vertex,
      .buffer = wgpu::BufferBindingLayout{
          .type = wgpu::BufferBindingType::Uniform,
          .hasDynamicOffset = true,
          .minBindingSize = sizeof(Math::Matrix4x4f),
      }};

  wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
      .entryCount = 1, .entries = &binding_layout_entry};

  wgpu::BindGroupLayout bind_group_layout =
      Engine::get_module<Graphics>()->device.CreateBindGroupLayout(
          &bind_group_layout_desc);

  wgpu::PipelineLayoutDescriptor layout_desc{
      .bindGroupLayoutCount = 1, .bindGroupLayouts = &bind_group_layout};

  wgpu::PipelineLayout pipeline_layout =
      Engine::get_module<Graphics>()->device.CreatePipelineLayout(&layout_desc);

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .layout = pipeline_layout,
      .vertex = {.module = shader_module,
                 .entryPoint = "vertexMain",
                 .bufferCount = 1,
                 .buffers = &vertex_buffer_layout},
      .fragment = &fragment_state};

  render_pipeline = Engine::get_module<Graphics>()->device.CreateRenderPipeline(
      &render_pipeline_desc);

  // constant buffer heap
  {
    auto bind_stride = ceil_to_next_multiple(
        sizeof(Math::Matrix4x4f),
        Engine::get_module<Graphics>()->limits.minUniformBufferOffsetAlignment);

    wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "constant buffer heap",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
        .size = bind_stride * max_instance_count,
        .mappedAtCreation = false,
    };

    constant_buffer_heap =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
  }

  wgpu::BindGroupEntry binding{.binding = 0,
                               .buffer = constant_buffer_heap,
                               .offset = 0,
                               .size = sizeof(Math::Matrix4x4f)};

  wgpu::BindGroupDescriptor bind_group_desc{
      .layout = bind_group_layout,
      .entryCount = bind_group_layout_desc.entryCount,
      .entries = &binding,
  };

  constant_buffer_bind_group =
      Engine::get_module<Graphics>()->device.CreateBindGroup(&bind_group_desc);

  // test texture gen
  wgpu::TextureDescriptor texture_desc{
      .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
      .dimension = wgpu::TextureDimension::e2D,
      .size = {256, 256, 1},
      .format = wgpu::TextureFormat::RGBA8Unorm,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
  };

  test_texture =
      Engine::get_module<Graphics>()->device.CreateTexture(&texture_desc);

  std::vector<uint8_t> pixels(4 * texture_desc.size.width *
                              texture_desc.size.height);
  for (uint32_t i = 0; i < texture_desc.size.width; i++) {
    for (uint32_t j = 0; j < texture_desc.size.height; j++) {
      uint32_t idx = 4 * (j * texture_desc.size.width + 1);
      pixels.at(idx + 0) = (uint8_t)i;
      pixels.at(idx + 1) = (uint8_t)j;
      pixels.at(idx + 2) = 128;
      pixels.at(idx + 3) = 255;
    }
  }

  wgpu::ImageCopyTexture destination{
      .texture = test_texture,
      .mipLevel = 0,
      .origin = {0, 0, 0},
      .aspect = wgpu::TextureAspect::All,
  };

  wgpu::TextureDataLayout source{
      .offset = 0,
      .bytesPerRow = 4 * texture_desc.size.width,
      .rowsPerImage = texture_desc.size.height,
  };

  Engine::get_module<Graphics>()->device.GetQueue().WriteTexture(
      &destination, pixels.data(), pixels.size(), &source, &texture_desc.size);
}

void ScreenSpaceMeshRenderer::render(wgpu::TextureView render_target) {
  // update constant
  int count = 0;
  for (const auto& mesh_wptr : mesh_list) {
    if (const auto& transform =
            mesh_wptr.lock()->entity->get_component<ScreenSpaceTransform>()) {
      const auto rotate = Math::Matrix4x4f(
          {{{std::cos(transform->angle), -std::sin(transform->angle), 0, 0},
            {std::sin(transform->angle), std::cos(transform->angle), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}}});
      const auto translate =
          Math::Matrix4x4f({{{1, 0, 0, transform->position.internal_data.at(0)},
                             {0, 1, 0, transform->position.internal_data.at(1)},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1}}});
      const auto scale =
          Math::Matrix4x4f({{{transform->scale.internal_data.at(0), 0, 0, 0},
                             {0, transform->scale.internal_data.at(1), 0, 0},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1}}});
      const auto matrix = translate * rotate * scale;
      Engine::get_module<Graphics>()->update_buffer(
          constant_buffer_heap, std::vector(1, transposed(matrix)),
          count * 256);
      count++;
    }
  }

  // render
  wgpu::RenderPassColorAttachment attachment{.view = render_target,
                                             .loadOp = wgpu::LoadOp::Clear,
                                             .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDescriptor renderpass_desc{.colorAttachmentCount = 1,
                                             .colorAttachments = &attachment};

  wgpu::CommandEncoder encoder =
      Engine::get_module<Graphics>()->device.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass_desc);
  pass.SetPipeline(render_pipeline);

  count = 0;
  for (const auto& mesh_wptr : mesh_list) {
    if (const auto& mesh = mesh_wptr.lock()) {
      pass.SetVertexBuffer(0, mesh->vertex_buffer, 0,
                           mesh->vertices.size() * sizeof(ScreenSpaceVertex));

      pass.SetIndexBuffer(mesh->index_buffer, wgpu::IndexFormat::Uint32, 0,
                          mesh->indices.size() * sizeof(uint32_t));

      uint32_t dynamic_offset =
          count *
          ceil_to_next_multiple(sizeof(Math::Matrix4x4f),
                                Engine::get_module<Graphics>()
                                    ->limits.minUniformBufferOffsetAlignment);
      pass.SetBindGroup(0, constant_buffer_bind_group, 1, &dynamic_offset);
      pass.DrawIndexed(mesh->indices.size(), 1, 0, 0, 0);
      count++;
    }
  }
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  Engine::get_module<Graphics>()->device.GetQueue().Submit(1, &commands);
}