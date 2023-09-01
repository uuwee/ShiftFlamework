#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif
#include <iostream>
#include <string>

namespace ShiftFlamework {
struct WindowDescriptor {
  std::string name;
  uint32_t height;
  uint32_t width;
};
class Window {
 private:
  GLFWwindow* window;
  wgpu::Surface surface;

 public:
  bool initialize(WindowDescriptor window_desc, wgpu::Instance& instance) {
    if (!glfwInit()) {
      std::cerr << "failed to initialize glfw" << std::endl;
      return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(window_desc.width, window_desc.height,
                              window_desc.name.c_str(), nullptr, nullptr);

#if defined(__EMSCRIPTEN__)
    wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvas_desc{};
    canvas_desc.selector = "#canvas";
    wgpu::SurfaceDescriptor surface_desc{.nextInChain = &canvas_desc};
    surface = instance.CreateSurface(&surface_desc);
#else
    surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
#endif
    return true;
  }

  wgpu::Surface get_surface() { return surface; }
  bool window_should_close() { return glfwWindowShouldClose(window); }
};
}  // namespace ShiftFlamework
