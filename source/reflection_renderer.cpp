#include "reflection_renderer.hpp"

#include <cassert>

#include "engine.hpp"
#include "graphics.hpp"
#include "matrix.hpp"
#include "mesh.hpp"
#include "vector.hpp"

using namespace SF;

RenderShader define_diffuse_pass() {
  wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
  wgsl_desc.code = R"(
    @group(0) @binding(0) var<uniform> world_mat: mat4x4f;
    @group(1) @binding(0) var<uniform> view_proj_mat: mat4x4f;
    @group(2) @binding(0) var texture: texture_2d<f32>;
    @group(2) @binding(1) var texture_sampler: sampler;

    struct VertexInput {
      @location(0) position: vec4f,
      @location(1) normal: vec3f,
      @location(2) tangent: vec4f,
      @location(3) uv: vec2f,
    };

    struct VertexOutput {
      @builtin(position) position: vec4f,
      @location(0) uv: vec2f,
      @location(1) normal: vec3f,
    };

    @vertex fn vertex_main(in: VertexInput) -> VertexOutput {
      var out: VertexOutput:
      out.position = view_proj_mat * world_mat * in.position;
      out.uv = in.uv;
      out.normal = in.normal;
      return out;
    };

    @fragment fn fragment_main(in: VertexOutput) -> @location(0) vec4f {
      return textureSample(texture, texture_sampler, in.uv);
    };
  )";

  wgpu::ShaderModuleDescriptor shader_module_desc{
      .nextInChain = &wgsl_desc,
      .label = "diffuse_pass",
  };
  wgpu::ShaderModule shader_module =
      Engine::get_module<Graphics>()->get_device().CreateShaderModule(
          &shader_module_desc);

  std::vector<wgpu::VertexAttribute> vertex_attributes(2);
  auto vertex_attribute_size = 0u;
  {
    // position
    vertex_attributes[0] = wgpu::VertexAttribute{
        .format = wgpu::VertexFormat::Float32x4,
        .offset = vertex_attribute_size,
        .shaderLocation = 0,
    };
    vertex_attribute_size += sizeof(Math::Vector4f) * 4;

    // normal
    vertex_attributes[1] = wgpu::VertexAttribute{
        .format = wgpu::VertexFormat::Float32x3,
        .offset = vertex_attribute_size,
        .shaderLocation = 1,
    };
    vertex_attribute_size += sizeof(Math::Vector3f) * 3;

    // tangent
    vertex_attributes[2] = wgpu::VertexAttribute{
        .format = wgpu::VertexFormat::Float32x4,
        .offset = vertex_attribute_size,
        .shaderLocation = 2,
    };
    vertex_attribute_size += sizeof(Math::Vector4f) * 4;

    // uv
    vertex_attributes[2] = wgpu::VertexAttribute{
        .format = wgpu::VertexFormat::Float32x2,
        .offset = vertex_attribute_size,
        .shaderLocation = 3,
    };
    vertex_attribute_size += sizeof(Math::Vector2f) * 2;

    assert(vertex_attribute_size == sizeof(Vertex));
  }

  wgpu::VertexBufferLayout vertex_buffer_layout{
      .arrayStride = vertex_attribute_size,
      .stepMode = wgpu::VertexStepMode::Instance,
      .attributeCount = static_cast<uint32_t>(vertex_attributes.size()),
      .attributes = vertex_attributes.data(),
  };

  std::vector<wgpu::BlendState> blend_states(1);
  blend_states.at(0) = wgpu::BlendState{
      .color =
          wgpu::BlendComponent{
              .operation = wgpu::BlendOperation::Add,
              .srcFactor = wgpu::BlendFactor::SrcAlpha,
              .dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha,
          },
      .alpha =
          wgpu::BlendComponent{
              .operation = wgpu::BlendOperation::Add,
              .srcFactor = wgpu::BlendFactor::SrcAlpha,
              .dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha,
          },
  };

  wgpu::ColorTargetState color_target_state{
      .format = wgpu::TextureFormat::BGRA8Unorm,
      .blend = &blend_states.at(0),
  };

  wgpu::FragmentState fragment_state{
      .module = shader_module,
      .entryPoint = "fragment_main",
      .targetCount = 1,
      .targets = &color_target_state,
  };

  auto binding_layout_entries = std::vector<wgpu::BindGroupLayoutEntry>(4);
  // world_mat
  binding_layout_entries[0] = wgpu::BindGroupLayoutEntry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Vertex,
      .buffer =
          wgpu::BufferBindingLayout{
              .type = wgpu::BufferBindingType::Uniform,
              .hasDynamicOffset = false,
              .minBindingSize = sizeof(Math::Matrix4x4f),
          },
  };

  // view_proj_mat
  binding_layout_entries[1] = wgpu::BindGroupLayoutEntry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Vertex,
      .buffer =
          wgpu::BufferBindingLayout{
              .type = wgpu::BufferBindingType::Uniform,
              .hasDynamicOffset = false,
              .minBindingSize = sizeof(Math::Matrix4x4f),
          },
  };

  // texture
  binding_layout_entries[2] = wgpu::BindGroupLayoutEntry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Fragment,
      .texture =
          wgpu::TextureBindingLayout{
              .sampleType = wgpu::TextureSampleType::Float,
              .viewDimension = wgpu::TextureViewDimension::e2D,
              .multisampled = false,
          },
  };

  // texture_sampler
  binding_layout_entries[3] = wgpu::BindGroupLayoutEntry{
      .binding = 1,
      .visibility = wgpu::ShaderStage::Fragment,
      .sampler =
          wgpu::SamplerBindingLayout{
              .type = wgpu::SamplerBindingType::Filtering,
          },
  };

  auto world_mat_binding_layout_desc = wgpu::BindGroupLayoutDescriptor{
      .entryCount = 1,
      .entries = &binding_layout_entries[0],
  };
  auto world_mat_binding_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &world_mat_binding_layout_desc);

  auto view_proj_mat_binding_layout_desc = wgpu::BindGroupLayoutDescriptor{
      .entryCount = 1,
      .entries = &binding_layout_entries[1],
  };
  auto view_proj_mat_binding_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &view_proj_mat_binding_layout_desc);

  auto texture_binding_layout_desc = wgpu::BindGroupLayoutDescriptor{
      .entryCount = 2,
      .entries =
          std::vector<wgpu::BindGroupLayoutEntry>{binding_layout_entries.at(2),
                                                  binding_layout_entries.at(3)}
              .data(),
  };
  auto texture_binding_layout =
      Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
          &texture_binding_layout_desc);

  auto pipeline_layout_desc = wgpu::PipelineLayoutDescriptor{
      .bindGroupLayoutCount = 3,
      .bindGroupLayouts =
          std::vector<wgpu::BindGroupLayout>{world_mat_binding_layout,
                                             view_proj_mat_binding_layout,
                                             texture_binding_layout}
              .data(),
  };
  auto pipeline_layout =
      Engine::get_module<Graphics>()->get_device().CreatePipelineLayout(
          &pipeline_layout_desc);

  wgpu::DepthStencilState depth_stencil_state{
      .nextInChain = nullptr,
      .format = wgpu::TextureFormat::Depth24Plus,
      .depthWriteEnabled = true,
      .depthCompare = wgpu::CompareFunction::Less,
      .stencilFront =
          wgpu::StencilFaceState{
              .compare = wgpu::CompareFunction::Always,
              .failOp = wgpu::StencilOperation::Keep,
              .depthFailOp = wgpu::StencilOperation::Keep,
              .passOp = wgpu::StencilOperation::Keep,
          },
      .stencilBack =
          wgpu::StencilFaceState{
              .compare = wgpu::CompareFunction::Always,
              .failOp = wgpu::StencilOperation::Keep,
              .depthFailOp = wgpu::StencilOperation::Keep,
              .passOp = wgpu::StencilOperation::Keep,
          },
      .stencilReadMask = 0,
      .stencilWriteMask = 0,
      .depthBias = 0,
      .depthBiasSlopeScale = 0,
      .depthBiasClamp = 0,
  };

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .layout = pipeline_layout,
      .vertex =
          {
              .module = shader_module,
              .entryPoint = "vertex_main",
              .bufferCount = 1,
              .buffers = &vertex_buffer_layout,
          },
      .depthStencil = &depth_stencil_state,
      .fragment = &fragment_state,
  };

  auto render_pipeline =
      Engine::get_module<Graphics>()->get_device().CreateRenderPipeline(
          &render_pipeline_desc);

  return RenderShader{
      .render_pipeline = render_pipeline,
      .pipeline_layout = pipeline_layout,
      .bind_group_layouts = {world_mat_binding_layout,
                             view_proj_mat_binding_layout,
                             texture_binding_layout},
  };
}



void ReflectionRenderer::initialize() {}

void ReflectionRenderer::render(wgpu::TextureView renderTarget) {}