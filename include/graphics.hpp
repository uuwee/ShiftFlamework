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
};
}  // namespace ShiftFlamework