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

#include "engine.hpp"

using namespace ShiftFlamework;

void render() {
  wgpu::RenderPassColorAttachment attachment{
      .view =
          Engine::GetModule<Window>()->get_swap_chain().GetCurrentTextureView(),
      .loadOp = wgpu::LoadOp::Clear,
      .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                        .colorAttachments = &attachment};

  wgpu::CommandEncoder encoder =
      Engine::GetModule<Graphics>()->device.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
  pass.SetPipeline(Engine::GetModule<Graphics>()->pipeline);
  pass.Draw(3);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  Engine::GetModule<Graphics>()->device.GetQueue().Submit(1, &commands);
}

void main_loop() { render(); }

void start() {
  {
    Window window("game window", 512, 512);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  Engine::GetModule<Window>()->initialize_swap_chain(
      Engine::GetModule<Graphics>()->instance,
      Engine::GetModule<Graphics>()->device);
  Engine::GetModule<Graphics>()->create_render_pipeline();
  Engine::GetModule<Window>()->start_main_loop(main_loop);
}

std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>> Engine::modules;

int main() {
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  Engine::GetModule<Graphics>()->instance = wgpu::CreateInstance();
  Engine::GetModule<Graphics>()->initialize([]() { start(); });
}