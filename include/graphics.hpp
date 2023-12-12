#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <iostream>
#include <memory>

namespace ShiftFlamework {

class Graphics {
 private:
  wgpu::Instance instance = nullptr;
  wgpu::Device device = nullptr;
  wgpu::SwapChain swap_chain = nullptr;
  wgpu::RenderPipeline pipeline = nullptr;
  uint32_t bind_stride;
  wgpu::Limits limits{};
  std::function<void()> on_initialize_end;

 public:
  void initialize(std::function<void()> on_initialize_end);

  void terminate() { device.Destroy(); }
  wgpu::Buffer create_buffer(const wgpu::BufferDescriptor& descriptor);

  uint32_t get_buffer_stride(uint32_t size);

  void set_device(wgpu::Device device);
  wgpu::Device get_device();
  void set_on_initialize_end(std::function<void()> on_initialize_end);
  void call_initialize_end();
  wgpu::Instance get_instance();

  wgpu::Limits get_limits();
  void set_limits(wgpu::Limits limits);

  template <typename T>
  void update_buffer(wgpu::Buffer& buffer, std::vector<T> data,
                     uint64_t offset = 0) {
    device.GetQueue().WriteBuffer(buffer, offset,
                                  reinterpret_cast<const uint8_t*>(data.data()),
                                  data.size() * sizeof(T));
  }
};
}  // namespace ShiftFlamework