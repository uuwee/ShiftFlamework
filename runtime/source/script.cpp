#include <windows.h>

#include <iostream>

#include "engine.hpp"
#include "test_image.h"

using namespace ShiftFlamework;
extern "C" __declspec(dllexport) void on_start() {
  std::cout << "Hello World!" << std::endl;

  auto e = ShiftFlamework::create_entity();
  e->add_component<ScreenSpaceMesh>();
  e->add_component<ScreenSpaceTransform>();
  auto mat = e->add_component<Material>();
  mat->create_gpu_buffer(test_image_height, test_image_width, test_image_data);
}