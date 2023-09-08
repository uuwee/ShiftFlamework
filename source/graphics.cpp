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