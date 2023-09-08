#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <iostream>
#include <memory>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

namespace ShiftFlamework {
class Graphics {
 public:
  wgpu::Instance instance = nullptr;
  wgpu::Adapter adapter = nullptr;
  wgpu::Device device = nullptr;
  wgpu::SwapChain swap_chain = nullptr;
  wgpu::RenderPipeline pipeline = nullptr;

  void initialize(std::function<void()> on_initialize_end);
  std::function<void()> on_initialize_end;

  void create_render_pipeline();
  void render(wgpu::TextureView current_texture_view) {
    wgpu::RenderPassColorAttachment attachment{.view = current_texture_view,
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
};
}  // namespace ShiftFlamework