#include "reflection_renderer_pass.hpp"

#include <vector>
#include "vector.hpp"
#include "matrix.hpp"

namespace SF {
DiffusePass create_diffuse_pass(Graphics& graphics) {
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
        graphics.get_device().CreateShaderModule(
            &shader_module_desc);

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
    binding_layout_entries.at(0) = wgpu::BindGroupLayoutEntry{
        .binding = 0,
        .visibility = wgpu::ShaderStage::Vertex,
        .buffer = wgpu::BufferBindingLayout{
            .type = wgpu::BufferBindingType::Uniform,
            .hasDynamicOffset = false,
            .minBindingSize = sizeof(Math::Matrix4x4f),
        }};

    // camera constant
    binding_layout_entries.at(1) = wgpu::BindGroupLayoutEntry{
        .binding = 0,
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
        .entryCount =
            static_cast<uint32_t>(mesh_constant_layout_entries.size()),
        .entries = mesh_constant_layout_entries.data(),
    };
    auto mesh_constant_bind_group_layout =
        graphics.get_device().CreateBindGroupLayout(
            &mesh_constant_layout_desc);

    std::vector<wgpu::BindGroupLayoutEntry> camera_layout_entries(
        binding_layout_entries.begin() + 1, binding_layout_entries.begin() + 2);
    wgpu::BindGroupLayoutDescriptor camera_layout_desc = {
        .entryCount = static_cast<uint32_t>(camera_layout_entries.size()),
        .entries = camera_layout_entries.data(),
    };
    auto camera_constant_bind_group_layout =
        graphics.get_device().CreateBindGroupLayout(
            &camera_layout_desc);

    std::vector<wgpu::BindGroupLayoutEntry> texture_layout_entries(
        binding_layout_entries.begin() + 2, binding_layout_entries.begin() + 4);
    wgpu::BindGroupLayoutDescriptor texture_layout_desc = {
        .entryCount = static_cast<uint32_t>(texture_layout_entries.size()),
        .entries = texture_layout_entries.data(),
    };
    auto texture_bind_group_layout =
        graphics.get_device().CreateBindGroupLayout(
            &texture_layout_desc);

    std::vector<wgpu::BindGroupLayout> layouts = {
        mesh_constant_bind_group_layout,
        camera_constant_bind_group_layout,
        texture_bind_group_layout};
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
        graphics.get_device().CreateTexture(
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

    auto depth_texture_view = depthTexture.CreateView(&depthTextureViewDesc);

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

    return DiffusePass{
        .render_pipeline = render_pipeline,
        .mesh_constant_bind_group_layout = mesh_constant_bind_group_layout,
        .camera_constant_bind_group_layout = camera_constant_bind_group_layout,
        .texture_bind_group_layout = texture_bind_group_layout,
    };
}
}  // namespace SF