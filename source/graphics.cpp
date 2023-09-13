#include "graphics.hpp"

#include "engine.hpp"

using namespace ShiftFlamework;

void get_device(wgpu::Instance instance, void (*callback)(wgpu::Device)) {
  instance.RequestAdapter(
      nullptr,
      [](WGPURequestAdapterStatus status, WGPUAdapter c_adapter,
         const char* message, void* userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
          std::cerr << "failed to get adapter" << std::endl;
          return;
        }
        wgpu::Adapter adapter = wgpu::Adapter::Acquire(c_adapter);
        Engine::GetModule<Graphics>()->adapter = adapter;
        adapter.RequestDevice(
            nullptr,
            [](WGPURequestDeviceStatus status, WGPUDevice c_device,
               const char* message, void* userdata) {
              wgpu::Device device = wgpu::Device::Acquire(c_device);
              reinterpret_cast<void (*)(wgpu::Device)>(userdata)(device);
            },
            userdata);
      },
      reinterpret_cast<void*>(callback));
}

void on_device_request_ended(wgpu::Device device) {
  Engine::GetModule<Graphics>()->device = device;
  Engine::GetModule<Graphics>()->on_initialize_end();
}

void Graphics::initialize(std::function<void()> on_initialize_end) {
  instance = wgpu::CreateInstance();
  this->on_initialize_end = on_initialize_end;
  get_device(instance, on_device_request_ended);
}

void Graphics::create_render_pipeline() {
  wgpu::SupportedLimits limits{};
  device.GetLimits(&limits);
  std::cout << "vertex attribute" << limits.limits.maxVertexAttributes
            << std::endl;

  vertex_data = {
      -0.5, -0.5, +0.5, -0.5, +0.0, +0.5,
  };
  int vertex_count = static_cast<int>(vertex_data.size() / 2);
  wgpu::BufferDescriptor buffer_desc{
      .nextInChain = nullptr,
      .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
      .size = vertex_data.size() * sizeof(float),
      .mappedAtCreation = false};
  vertex_buffer = device.CreateBuffer(&buffer_desc);

  device.GetQueue().WriteBuffer(vertex_buffer, 0, vertex_data.data(),
                                buffer_desc.size);

  wgpu::VertexAttribute vertex_attribute{
      .format = wgpu::VertexFormat::Float32x2,
      .offset = 0,
      .shaderLocation = 0};
  wgpu::VertexBufferLayout vertex_buffer_layout{
      .arrayStride = 2 * sizeof(float),
      .stepMode = wgpu::VertexStepMode::Vertex,
      .attributeCount = 1,
      .attributes = &vertex_attribute,
  };

  wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
  wgsl_desc.code = R"(
      @vertex fn vertexMain(@location(0) in_vertex_position: vec2f) -> @builtin(position) vec4f {
        return vec4f(in_vertex_position, 0.0, 1.0);
      }
      @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
      }
    )";

  wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
  wgpu::ShaderModule shader_module =
      device.CreateShaderModule(&shader_module_desc);

  wgpu::ColorTargetState color_target_state{
      .format = wgpu::TextureFormat::BGRA8Unorm};
  wgpu::FragmentState fragment_state{.module = shader_module,
                                     .entryPoint = "fragmentMain",
                                     .targetCount = 1,
                                     .targets = &color_target_state};

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .vertex = {.module = shader_module,
                 .entryPoint = "vertexMain",
                 .bufferCount = 1,
                 .buffers = &vertex_buffer_layout},
      .fragment = &fragment_state};

  pipeline = device.CreateRenderPipeline(&render_pipeline_desc);
}

void Graphics::render(wgpu::TextureView current_texture_view) {
  wgpu::RenderPassColorAttachment attachment{.view = current_texture_view,
                                             .loadOp = wgpu::LoadOp::Clear,
                                             .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                        .colorAttachments = &attachment};

  wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
  pass.SetPipeline(pipeline);
  pass.SetVertexBuffer(0, vertex_buffer, 0, vertex_data.size() * sizeof(float));
  pass.Draw(vertex_data.size() / 2, 1, 0, 0);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  device.GetQueue().Submit(1, &commands);
}