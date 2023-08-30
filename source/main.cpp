#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <iostream>
#include <memory>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

#include "../include/vector3.hpp"
wgpu::Instance instance = nullptr;
wgpu::Device device = nullptr;

const uint32_t k_width = 512;
const uint32_t k_height = 512;

void start() {}

int main() {
  instance = wgpu::CreateInstance();

  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window =
      glfwCreateWindow(k_width, k_height, "WebGPU window", nullptr, nullptr);

#if defined(__EMSCRIPTEN__)
  wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  wgpu::Surface surface = instance.CreateSurface(&surfaceDesc);
#else
  wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
#endif

#if defined(__EMSCRIPTEN__)
  // emscripten_set_main_loop(Render)
#else
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
#endif
}