#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <functional>
#include <iostream>
#include <string>

namespace ShiftFlamework {

class Window {
 private:
  GLFWwindow* window;
  wgpu::Surface surface;
  wgpu::SwapChain swap_chain;
  const std::string name;
  const uint32_t width, height;
  std::function<void()> main_loop;

 public:
  Window(const std::string window_name, const uint32_t width,
         const uint32_t height);

  bool initialize_swap_chain(wgpu::Instance& instance, wgpu::Device& device);
  void start_main_loop(std::function<void()> main_loop);
  const wgpu::SwapChain& get_swap_chain() { return swap_chain; }
  void call_main_loop() { main_loop(); }
};
}  // namespace ShiftFlamework
