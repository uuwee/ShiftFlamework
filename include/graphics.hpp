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
  wgpu::Instance instance;
  wgpu::Device device;
  wgpu::SwapChain swap_chain;
  wgpu::RenderPipeline pipeline;
};
}  // namespace ShiftFlamework