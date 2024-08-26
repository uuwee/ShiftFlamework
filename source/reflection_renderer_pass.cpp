#include "reflection_renderer_pass.hpp"

#include <vector>

#include "matrix.hpp"
#include "vector.hpp"

namespace SF {
DiffusePass create_diffuse_pass(Graphics& graphics,
                                wgpu::Texture depth_texture) {
  std::vector<wgpu::VertexAttribute> vertex_attributes{
      wgpu::VertexAttribute{
          .format = wgpu::VertexFormat::Float32x4,
          .offset = 0,
          .shaderLocation = 0,
      },
      wgpu::VertexAttribute{
          .format = wgpu::VertexFormat::Float32x3,
          .offset = 4 * 4,  // 4 * sizeof(Float32)
          .shaderLocation = 1,
      },
      wgpu::VertexAttribute{
          .format = wgpu::VertexFormat::Float32x4,
          .offset = 7 * 4,  // (4 + 3)  * sizeof(Float32)
          .shaderLocation = 2,
      },
      wgpu::VertexAttribute{
          .format = wgpu::VertexFormat::Float32x2,
          .offset = 11 * 4,  // (4 + 3 + 4) * sizeof(Float32)
          .shaderLocation = 3,
      },
  };

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
    };

    @vertex fn vertexMain(in: VertexInput) -> VertexOutput{
        var out: VertexOutput;
        out.position = view_proj_mat * world_mat * in.position;
        // note that normalizing w is automatically done by the hardware
        // see https://eliemichel.github.io/LearnWebGPU/basic-3d-rendering/3d-meshes/projection-matrices.html#perspective-matrix

        out.texcoord0 = in.texcoord0;
        out.normal = in.normal;
        return out;
    };

    @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f{
        var p: vec4f;
        p = textureSample(tex, tex_sampler, in.texcoord0);
        return p;
    };
  )";

  wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
  wgpu::ShaderModule shader_module =
      graphics.get_device().CreateShaderModule(&shader_module_desc);

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
      .blend = blend_states.data(),
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
  auto camera_constant_bind_group_layout_entry = binding_layout_entries.at(1);

  std::vector<wgpu::BindGroupLayoutEntry> mesh_constant_layout_entries(
      binding_layout_entries.begin(), binding_layout_entries.begin() + 1);
  wgpu::BindGroupLayoutDescriptor mesh_constant_layout_desc = {
      .entryCount = static_cast<uint32_t>(mesh_constant_layout_entries.size()),
      .entries = mesh_constant_layout_entries.data(),
  };
  auto mesh_constant_bind_group_layout =
      graphics.get_device().CreateBindGroupLayout(&mesh_constant_layout_desc);

  std::vector<wgpu::BindGroupLayoutEntry> camera_layout_entries(
      binding_layout_entries.begin() + 1, binding_layout_entries.begin() + 2);
  wgpu::BindGroupLayoutDescriptor camera_layout_desc = {
      .entryCount = static_cast<uint32_t>(camera_layout_entries.size()),
      .entries = camera_layout_entries.data(),
  };
  auto camera_constant_bind_group_layout =
      graphics.get_device().CreateBindGroupLayout(&camera_layout_desc);

  std::vector<wgpu::BindGroupLayoutEntry> texture_layout_entries(
      binding_layout_entries.begin() + 2, binding_layout_entries.begin() + 4);
  wgpu::BindGroupLayoutDescriptor texture_layout_desc = {
      .entryCount = static_cast<uint32_t>(texture_layout_entries.size()),
      .entries = texture_layout_entries.data(),
  };
  auto texture_bind_group_layout =
      graphics.get_device().CreateBindGroupLayout(&texture_layout_desc);

  std::vector<wgpu::BindGroupLayout> layouts = {
      mesh_constant_bind_group_layout, camera_constant_bind_group_layout,
      texture_bind_group_layout};
  wgpu::PipelineLayoutDescriptor layout_desc{
      .bindGroupLayoutCount = static_cast<uint32_t>(layouts.size()),
      .bindGroupLayouts = layouts.data(),
  };

  wgpu::PipelineLayout pipeline_layout =
      graphics.get_device().CreatePipelineLayout(&layout_desc);

  wgpu::DepthStencilState depth_stencil_state{
      .nextInChain = nullptr,
      .format = depth_texture.GetFormat(),
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

  auto depth_texture_view = depth_texture.CreateView(&depthTextureViewDesc);

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .layout = pipeline_layout,
      .vertex = {.module = shader_module,
                 .entryPoint = "vertexMain",
                 .bufferCount = 1,
                 .buffers = &vertex_buffer_layout},
      .depthStencil = &depth_stencil_state,
      .fragment = &fragment_state,
  };

  auto render_pipeline =
      graphics.get_device().CreateRenderPipeline(&render_pipeline_desc);

  return DiffusePass{
      .render_pipeline = render_pipeline,
      .mesh_constant_bind_group_layout = mesh_constant_bind_group_layout,
      .camera_constant_bind_group_layout = camera_constant_bind_group_layout,
      .texture_bind_group_layout = texture_bind_group_layout,
      .depth_texture_view = depth_texture_view,
  };
}

AABBPass create_aabb_pass(Graphics& graphics) {
    std::vector<wgpu::VertexAttribute> vertex_attributes{
        wgpu::VertexAttribute{
            // position
            .format = wgpu::VertexFormat::Float32x4,
            .offset = 0,
            .shaderLocation = 0,
        },
        wgpu::VertexAttribute{
            // color
            .format = wgpu::VertexFormat::Float32x3,
            .offset = 4 * 4,  // 4 * sizeof(Float32)
            .shaderLocation = 1,
        },
    };

    wgpu::VertexBufferLayout vertex_buffer_layout{
        .arrayStride = 7 * 4,  // (4 + 3) * sizeof(Float32)
        .stepMode = wgpu::VertexStepMode::Vertex,
        .attributeCount = static_cast<uint32_t>(vertex_attributes.size()),
        .attributes = vertex_attributes.data()};

    wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
    wgsl_desc.code = R"(
    struct AABB{
        @align(16) min: vec3f,
        @align(16) max: vec3f,
    };
    @group(0) @binding(0)  var<uniform> aabb: AABB;
    @group(1) @binding(0)  var<uniform> view_proj_mat: mat4x4f;

    struct VertexInput{
      @location(0) position: vec4f,
      @location(1) color: vec3f,
    };

    struct VertexOutput{
        @builtin(position) position: vec4f,
        @location(0) color: vec3f,
    };

    @vertex fn vertexMain(in: VertexInput) -> VertexOutput{
        var out: VertexOutput;
        var pos: vec3f = in.position.xyz;
        let center = (aabb.max + aabb.min) / 2.0;
        let scale = (aabb.max - aabb.min) / 2.0;
        pos = (pos * scale) + center;
        out.position = view_proj_mat * vec4f(pos, 1.0);
        out.color = in.color;
        return out;
    };

    @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f{
        return vec4<f32>(in.color, 1.0);
    };
    )";

    wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
    wgpu::ShaderModule shader_module =
        graphics.get_device().CreateShaderModule(
            &shader_module_desc);

    std::vector<wgpu::BlendState> blend_states{wgpu::BlendState{
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
    }};

    wgpu::ColorTargetState color_target_state{
        .format = wgpu::TextureFormat::BGRA8Unorm,
        .blend = blend_states.data(),
    };

    wgpu::FragmentState fragment_state{
        .module = shader_module,
        .entryPoint = "fragmentMain",
        .targetCount = 1,
        .targets = &color_target_state,
    };

    // mesh constant
    auto gizmo_mesh_constant_bind_group_layout_entry =
        wgpu::BindGroupLayoutEntry{
            .binding = 0,
            .visibility = wgpu::ShaderStage::Vertex,
            .buffer =
                wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::Uniform,
                    .hasDynamicOffset = true,
                    .minBindingSize = sizeof(AABB),
                },
        };

    // camera
    auto gizmo_camera_constant_bind_group_layout_entry =
        wgpu::BindGroupLayoutEntry{
            .binding = 0,
            .visibility = wgpu::ShaderStage::Vertex,
            .buffer =
                wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::Uniform,
                    .hasDynamicOffset = false,
                    .minBindingSize = sizeof(Math::Matrix4x4f),
                },
        };

    wgpu::BindGroupLayoutDescriptor gizmo_mesh_constant_layout_desc = {
        .entryCount = 1,
        .entries = &gizmo_mesh_constant_bind_group_layout_entry,
    };
    auto aabb_bind_group_layout =
        graphics.get_device().CreateBindGroupLayout(
            &gizmo_mesh_constant_layout_desc);

    wgpu::BindGroupLayoutDescriptor camera_constant_layout_desc = {
        .entryCount = 1,
        .entries = &gizmo_camera_constant_bind_group_layout_entry,
    };
    auto camera_constant_bind_group_layout =
        graphics.get_device().CreateBindGroupLayout(
            &camera_constant_layout_desc);

    std::vector<wgpu::BindGroupLayout> layouts = {
        aabb_bind_group_layout,
        camera_constant_bind_group_layout,
    };
    wgpu::PipelineLayoutDescriptor layout_desc{
        .bindGroupLayoutCount = static_cast<uint32_t>(layouts.size()),
        .bindGroupLayouts = layouts.data(),
    };
    wgpu::PipelineLayout pipeline_layout =
        graphics.get_device().CreatePipelineLayout(
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

    wgpu::RenderPipelineDescriptor render_pipeline_desc{
        .layout = pipeline_layout,
        .vertex = {.module = shader_module,
                   .entryPoint = "vertexMain",
                   .bufferCount = 1,
                   .buffers = &vertex_buffer_layout},
        .depthStencil = &depth_stencil_state,
        .fragment = &fragment_state,
    };

    auto render_pipeline =
        graphics.get_device().CreateRenderPipeline(
            &render_pipeline_desc);

    return AABBPass{
        .render_pipeline = render_pipeline,
        .aabb_data_bind_group_layout = aabb_bind_group_layout,
        .camera_constant_bind_group_layout = camera_constant_bind_group_layout,
    };
}
}  // namespace SF