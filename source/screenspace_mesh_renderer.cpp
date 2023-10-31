#include "screenspace_mesh_renderer.hpp"

#include "engine.hpp"
#include "graphics.hpp"
#include "matrix.hpp"
#include "test_image.h"

using namespace ShiftFlamework;

uint32_t ceil_to_next_multiple(uint32_t value, uint32_t step) {
  uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
  return step * divide_and_ceil;
}

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
        return vec4f(textureSample(gradientTexture, textureSampler, in.texture_coord).rgb, 1.0);
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

  auto binding_layout_entries =
      std::vector<wgpu::BindGroupLayoutEntry>(3);
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

  std::vector<wgpu::BindGroupLayoutEntry> constant_layout_entries(
      binding_layout_entries.begin(), binding_layout_entries.begin() + 1);
  wgpu::BindGroupLayoutDescriptor constant_bind_group_layout_desc = {
      .entryCount = 1, .entries = constant_layout_entries.data()};
  constant_bind_group_layout =
      Engine::get_module<Graphics>()->device.CreateBindGroupLayout(
          &constant_bind_group_layout_desc);

  std::vector<wgpu::BindGroupLayoutEntry> texture_layout_entries(binding_layout_entries.begin() + 1,
                              binding_layout_entries.end());
  wgpu::BindGroupLayoutDescriptor texture_bind_group_layout_desc = {
      .entryCount = 2,
      .entries = texture_layout_entries.data(),
  };

  texture_bind_group_layout =
      Engine::get_module<Graphics>()->device.CreateBindGroupLayout(
          &texture_bind_group_layout_desc);
  std::vector<wgpu::BindGroupLayout> layouts = {constant_bind_group_layout,
                                                texture_bind_group_layout};
  wgpu::PipelineLayoutDescriptor layout_desc{
      .bindGroupLayoutCount = 2, .bindGroupLayouts = layouts.data()};

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
        .size = bind_stride,
        .mappedAtCreation = false,
    };

    constant_buffer_heap =
        Engine::get_module<Graphics>()->create_buffer(buffer_desc);
  }

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

  std::vector<uint8_t> pixels(
      4 * texture_desc.size.width * texture_desc.size.height, 0);

  {
    uint32_t idx = 0;
    for (uint32_t i = 0; i < width; i++) {
      for (uint32_t j = 0; j < height; j++) {
        std::array<uint8_t, 4> data = {};
        data.at(0) = (uint8_t)header_data[idx];
        data.at(1) = (uint8_t)header_data[idx + 1];
        data.at(2) = (uint8_t)header_data[idx + 2];
        data.at(3) = (uint8_t)header_data[idx + 3];
        idx += 4;

        pixels.at(4 * ((width - 1 - i) * height + j) + 0) =
            (((data.at(0) - 33) << 2) | ((data.at(1) - 33) >> 4));
        pixels.at(4 * ((width - 1 - i) * height + j) + 1) =
            ((((data.at(1) - 33) & 0xF) << 4) | ((data.at(2) - 33) >> 2));
        pixels.at(4 * ((width - 1 - i) * height + j) + 2) =
            ((((data.at(2) - 33) & 0x3) << 6) | ((data.at(3) - 33)));
      }
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

  auto texture_view_desc = wgpu::TextureViewDescriptor{
      .format = texture_desc.format,
      .dimension = wgpu::TextureViewDimension::e2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = wgpu::TextureAspect::All,
  };
  auto texture_view = test_texture.CreateView(&texture_view_desc);

  // sampler
  auto sampler_desc =
      wgpu::SamplerDescriptor{.addressModeU = wgpu::AddressMode::ClampToEdge,
                              .addressModeV = wgpu::AddressMode::ClampToEdge,
                              .addressModeW = wgpu::AddressMode::ClampToEdge,
                              .magFilter = wgpu::FilterMode::Linear,
                              .minFilter = wgpu::FilterMode::Linear,
                              .mipmapFilter = wgpu::MipmapFilterMode::Linear,
                              .lodMinClamp = 0.0f,
                              .lodMaxClamp = 1.0f,
                              .compare = wgpu::CompareFunction::Undefined,
                              .maxAnisotropy = 1};
  auto sampler =
      Engine::get_module<Graphics>()->device.CreateSampler(&sampler_desc);

  // bindings
  constant_buffer_bind_group = create_constant_bind_group(constant_buffer_heap);
  texture_bind_group = create_texture_bind_group(texture_view, sampler);
}

void ScreenSpaceMeshRenderer::render(wgpu::TextureView render_target) {
  // update constant
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
          constant_buffer_heap, std::vector(1, transposed(matrix)));
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

  for (const auto& mesh_wptr : mesh_list) {
    if (const auto& mesh = mesh_wptr.lock()) {
      pass.SetVertexBuffer(0, mesh->vertex_buffer, 0,
                           mesh->vertices.size() * sizeof(ScreenSpaceVertex));

      pass.SetIndexBuffer(mesh->index_buffer, wgpu::IndexFormat::Uint32, 0,
                          mesh->indices.size() * sizeof(uint32_t));

      pass.SetBindGroup(0, constant_buffer_bind_group, 0, nullptr);
      pass.SetBindGroup(1, texture_bind_group, 0, nullptr);
      pass.DrawIndexed(mesh->indices.size(), 1, 0, 0, 0);
    }
  }
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  Engine::get_module<Graphics>()->device.GetQueue().Submit(1, &commands);
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

  return Engine::get_module<Graphics>()->device.CreateBindGroup(
      &bind_group_desc);
}

wgpu::BindGroup ScreenSpaceMeshRenderer::create_texture_bind_group(
    const wgpu::TextureView& texture_view, const wgpu::Sampler& sampler) {
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
      .entryCount = 2,
      .entries = texture_bindings.data(),
  };
  return Engine::get_module<Graphics>()->device.CreateBindGroup(
      &texture_bind_group_desc);
}