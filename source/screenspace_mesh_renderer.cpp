#include "screenspace_mesh_renderer.hpp"

#include "graphics.hpp"
#include "matrix.hpp"

using namespace ShiftFlamework;

uint32_t ceil_to_next_multiple(uint32_t value, uint32_t step) {
  uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
  return step * divide_and_ceil;
}

void ScreenSpaceMeshRenderer::initialize() {
  auto graphics = Engine::get_module<Graphics>();
  auto bind_stride =
      ceil_to_next_multiple(sizeof(Math::Matrix4x4f),
                            graphics->limits.minUniformBufferOffsetAlignment);

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
      @group(0) @binding(0) var<uniform> uTime: mat4x4f;

      struct VertexInput{
        @location(0) position: vec2f,
        @location(1) color: vec3f,
      }

      struct VertexOutput{
        @builtin(position) position: vec4f,
        @location(0) color: vec3f,
      }

      @vertex fn vertexMain(in: VertexInput) -> VertexOutput {
        var out: VertexOutput;
        out.position = uTime * vec4f(in.position, 0.0, 1.0);
        out.position /= out.position.w;
        out.color = in.color; 
        return out;
      }

      @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f {
        return vec4f(in.color, 1.0);
      }
    )";
    wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
    wgpu::ShaderModule shader_module =
        graphics->device.CreateShaderModule(&shader_module_desc);

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
            .minBindingSize = sizeof(Math::Matrix3x3f),
        }};

    wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
        .entryCount = 1, .entries = &binding_layout_entry};

    wgpu::BindGroupLayout bind_group_layout =
        graphics->device.CreateBindGroupLayout(&bind_group_layout_desc);

    wgpu::PipelineLayoutDescriptor layout_desc{
        .bindGroupLayoutCount = 1, .bindGroupLayouts = &bind_group_layout};

    wgpu::PipelineLayout pipeline_layout =
        graphics->device.CreatePipelineLayout(&layout_desc);

    wgpu::RenderPipelineDescriptor render_pipeline_desc{
        .layout = pipeline_layout, .vertex = {.module = shader_module, .entryPoint = "vertexMain", .bufferCount = 1, .buffers = &vertex_buffer_layout}, .fragment = &fragment_state};

    render_pipeline =
        graphics->device.CreateRenderPipeline(&render_pipeline_desc);

    // constant buffer heap
    {
      wgpu::BufferDescriptor buffer_desc{
          .nextInChain = nullptr,
          .label = "constant buffer heap",
          .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
          .size = bind_stride * max_instance_count,
          .mappedAtCreation = false,
      };

      constant_buffer_heap = graphics->create_buffer(buffer_desc);
      //fill buffer with identity matrix
      graphics->update_buffer(
          constant_buffer_heap,
          std::vector<Math::Matrix3x3f>(
            max_instance_count,
              Math::Matrix3x3f({{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}})
              ));
    }

    wgpu::BindGroupEntry binding{.binding = 0,
                                 .buffer = constant_buffer_heap,
                                 .offset = 0,
                                 .size = sizeof(Math::Matrix3x3f)};

    wgpu::BindGroupDescriptor bind_group_desc{
        .layout = bind_group_layout,
        .entryCount = bind_group_layout_desc.entryCount,
        .entries = &binding,
    };

    constant_buffer_bind_group =
        graphics->device.CreateBindGroup(&bind_group_desc);
}

void ScreenSpaceMeshRenderer::render(wgpu::TextureView render_target) {}