#pragma once
// #include <GLFW/glfw3.h>
// #include <webgpu/webgpu_cpp.h>

namespace wgpu {
typedef void* Instance;
typedef void* Adapter;
typedef void* Device;
typedef void* SwapChain;
typedef void* RenderPipeline;
typedef void* Buffer;
typedef void* BufferDescriptor;
typedef void* TextureView;
typedef void* BindGroup;
typedef void* Limits;
typedef void* Device;
typedef void* Texture;
typedef void* BindGroupLayout;
typedef void* Sampler;
}  // namespace wgpu

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "module.hpp"

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
  void terminate();
  wgpu::Buffer create_buffer(const wgpu::BufferDescriptor& descriptor);

  uint32_t get_buffer_stride(uint32_t size);

  template <typename T>
  void update_buffer(wgpu::Buffer& buffer, std::vector<T> data,
                     uint64_t offset = 0) {
    device.GetQueue().WriteBuffer(buffer, offset,
                                  reinterpret_cast<const uint8_t*>(data.data()),
                                  data.size() * sizeof(T));
  }
};
}  // namespace ShiftFlamework