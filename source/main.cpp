#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <iostream>
#include <memory>
#include <tuple>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

#include "../include/engine.hpp"

using namespace ShiftFlamework;

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
  std::get<std::shared_ptr<Window>>(Engine::modules) =
      std::make_shared<Window>();

  WindowDescriptor window_desc{
      .name = "Game Window",
      .height = k_height,
      .width = k_width,
  };
  Engine::GetModule<Window>()->initialize(window_desc, instance);

  init_graphics(Engine::GetModule<Window>()->get_surface());

#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop(render, 0, false);
#else
  while (!Engine::GetModule<Window>()->window_should_close()) {
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

std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>> Engine::modules;

int main() {
  instance = wgpu::CreateInstance();
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  Engine::GetModule<Graphics>()->instance = instance;
  get_device([](wgpu::Device dev) {
    device = dev;
    start();
  });
}