#include <windows.h>

#include <iostream>

#include "engine_import.hpp"
#include "test_image.h"

using namespace ShiftFlamework;

std::shared_ptr<Entity> e = nullptr;

extern "C" __declspec(dllexport) void on_start() {
  std::cout << "Hello World!" << std::endl;

  e = create_entity();
  e->add_component<ScreenSpaceMesh>();
  e->add_component<ScreenSpaceTransform>();
  auto mat = e->add_component<Material>();
  e->get_component<Material>()->create_gpu_buffer(
      test_image_height, test_image_width, test_image_data);
}

extern "C" __declspec(dllexport) void on_update() {
  std::cout << "update from on_update!" << std::endl;
}

extern "C" __declspec(dllexport) void on_end() {
  std::cout << "on_end" << std::endl;
  destroy_entity(e);
  e = nullptr;
}