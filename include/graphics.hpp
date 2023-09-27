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
  wgpu::Buffer vertex_buffer;
  wgpu::Buffer index_buffer;
  std::vector<float> vertex_data;
  std::vector<uint16_t> index_data;

  void initialize(std::function<void()> on_initialize_end);
  std::function<void()> on_initialize_end;

  void create_render_pipeline();
  void render(wgpu::TextureView current_texture_view);
  void terminate() { device.Destroy(); }
};
}  // namespace ShiftFlamework