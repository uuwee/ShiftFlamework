#include "graphics.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include "engine.hpp"
#include "matrix.hpp"
#include "vector.hpp"
using namespace SF;

uint32_t ceil_to_next_multiple(uint32_t value, uint32_t step) {
  uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
  return step * divide_and_ceil;
}

uint32_t Graphics::get_buffer_stride(uint32_t size) {
  return ceil_to_next_multiple(size, limits.minUniformBufferOffsetAlignment);
}

void request_device(wgpu::Instance instance, void (*callback)(wgpu::Device)) {
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

void on_device_request_ended(wgpu::Device device) {
  Engine::get_module<Graphics>()->set_device(device);
  Engine::get_module<Graphics>()->call_initialize_end();
}

void Graphics::initialize(std::function<void()> on_initialize_end) {
  instance = wgpu::CreateInstance();
  this->on_initialize_end = on_initialize_end;
  request_device(instance, on_device_request_ended);
}

void Graphics::set_device(wgpu::Device device) { this->device = device; }
wgpu::Device Graphics::get_device() { return device; }
void Graphics::set_on_initialize_end(std::function<void()> on_initialize_end) {
  this->on_initialize_end = on_initialize_end;
}
void Graphics::call_initialize_end() { on_initialize_end(); }
wgpu::Instance Graphics::get_instance() { return instance; }

wgpu::Limits Graphics::get_limits() { return limits; }
void Graphics::set_limits(wgpu::Limits limits) { this->limits = limits; }

wgpu::Buffer Graphics::create_buffer(const wgpu::BufferDescriptor& descriptor) {
  return device.CreateBuffer(&descriptor);
}