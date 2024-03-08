#include <windows.h>

#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#include "engine_import.hpp"
#include "test_image.h"

using namespace ShiftFlamework;

std::shared_ptr<Entity> e = nullptr;
std::queue<std::shared_ptr<Entity>> entities = {};

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
  std::cout << "#entity: " << entities.size() << std::endl;
  {
    auto tmp = create_entity();
    tmp->add_component<ScreenSpaceMesh>();
    tmp->add_component<ScreenSpaceTransform>();
    auto mat = tmp->add_component<Material>();
    tmp->get_component<Material>()->create_gpu_buffer(
        test_image_height, test_image_width, test_image_data);
    entities.push(tmp);
  }
  if (entities.size() > 20) {
    std::cout << "tmp: " << entities.front() << std::endl;
    std::cout << "e: " << e << std::endl;
    destroy_entity(entities.front());
    entities.pop();
  }
}

extern "C" __declspec(dllexport) void on_end() {
  std::cout << "on_end" << std::endl;
  destroy_entity(e);
  e = nullptr;
}