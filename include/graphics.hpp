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
  wgpu::Buffer time_buffer;
  std::vector<float> vertex_data;
  std::vector<uint16_t> index_data;
  float time_data = 0.0f;
  wgpu::BindGroup bind_group;
  uint32_t bind_stride;
  wgpu::Limits limits{};

  void initialize(std::function<void()> on_initialize_end);
  std::function<void()> on_initialize_end;

  void create_render_pipeline();
  void render(wgpu::TextureView current_texture_view);
  void terminate() { device.Destroy(); }
  wgpu::Buffer create_buffer(const wgpu::BufferDescriptor& descriptor) {
    return device.CreateBuffer(&descriptor);
  }

  template <typename T>
  void update_buffer(wgpu::Buffer& buffer, std::vector<T> data) {
    device.GetQueue().WriteBuffer(buffer, 0, data.data(),
                                  data.size() * sizeof(T));
  }
};
}  // namespace ShiftFlamework