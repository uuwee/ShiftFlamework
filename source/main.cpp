#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <iostream>
#include <memory>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

wgpu::Instance instance;
wgpu::Device device;
wgpu::SwapChain swap_chain;
wgpu::RenderPipeline pipeline;

const uint32_t k_width = 512;
const uint32_t k_height = 512;

const char shader_code[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";

void setup_swap_chain(wgpu::Surface surface) {
  wgpu::SwapChainDescriptor sc_desc{
      .usage = wgpu::TextureUsage::RenderAttachment,
      .format = wgpu::TextureFormat::BGRA8Unorm,
      .width = k_width,
      .height = k_height,
      .presentMode = wgpu::PresentMode::Fifo};
  swap_chain = device.CreateSwapChain(surface, &sc_desc);
}

void create_render_pipeline() {
  wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
  wgsl_desc.code = shader_code;

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
      .vertex = {.module = shader_module, .entryPoint = "vertexMain"},
      .fragment = &fragment_state};
  pipeline = device.CreateRenderPipeline(&render_pipeline_desc);
}

void init_graphics(wgpu::Surface surface) {
  setup_swap_chain(surface);
  create_render_pipeline();
}

void render() {
  wgpu::RenderPassColorAttachment attachment{
      .view = swap_chain.GetCurrentTextureView(),
      .loadOp = wgpu::LoadOp::Clear,
      .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                        .colorAttachments = &attachment};

  wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
  pass.SetPipeline(pipeline);
  pass.Draw(3);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  device.GetQueue().Submit(1, &commands);
}

void start() {
  if (!glfwInit()) {
    std::cerr << "failed to initialize glfw" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window =
      glfwCreateWindow(k_width, k_height, "WebGPU window", nullptr, nullptr);
#if defined(__EMSCRIPTEN__)
  wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  wgpu::Surface surface = instance.CreateSurface(&surfaceDesc);
#else
  wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
#endif

  init_graphics(surface);

#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop(render, 0, false);
#else
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    render();
    swap_chain.Present();
  }
#endif
}

void get_device(void (*callback)(wgpu::Device)) {
  instance.RequestAdapter(
      nullptr,
      [](WGPURequestAdapterStatus status, WGPUAdapter c_adapter,
         const char* message, void* userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
          std::cerr << "failed to get adapter" << std::endl;
          return;
        }
        wgpu::Adapter adapter = wgpu::Adapter::Acquire(c_adapter);
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

int main() {
  instance = wgpu::CreateInstance();
  get_device([](wgpu::Device dev) {
    device = dev;
    start();
  });
}