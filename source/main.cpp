#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "material.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_mesh_renderer.hpp"
#include "test_image.h"
#include "vector.hpp"
#include "window.hpp"
using namespace ShiftFlamework;

void main_loop() {
  // user script

  Engine::get_module<Input>()->update();
  Engine::get_module<ScreenSpaceMeshRenderer>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 1080, 1080);
    Engine::add_module(std::make_shared<Window>(window));
  }

  wgpu::SupportedLimits supported_limits;
  Engine::get_module<Graphics>()->get_device().GetLimits(&supported_limits);
  Engine::get_module<Graphics>()->set_limits(supported_limits.limits);

  Engine::get_module<Window>()->initialize_swap_chain(
      Engine::get_module<Graphics>()->get_instance(),
      Engine::get_module<Graphics>()->get_device());
  Engine::get_module<ScreenSpaceMeshRenderer>()->initialize();

  // game initialize
  // auto e = std::make_shared<Entity>();
  // e->add_component<ScreenSpaceMesh>();
  // e->add_component<ScreenSpaceTransform>();
  // auto mat = e->add_component<Material>();
  // mat->create_gpu_buffer(test_image_height, test_image_width,
  // test_image_data);

  auto e = std::shared_ptr<Entity>(
      (Entity*)ShiftFlamework_Entity_Constructor(),
      [&](void* ptr) { ((Entity*)ptr)->remove_reference(); });
  ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceMesh(e.get());
  ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceTransform(
      e.get());
  auto mat =
      (Material*)ShiftFlamework_Entity_add_component_ShiftFlamework_Material(
          e.get());

  ShiftFlamework_Material_create_gpu_buffer(mat, test_image_height,
                                            test_image_width, test_image_data);

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

int main() {
  // initialize modules
  Engine::add_module<Graphics>();
  Engine::add_module<Input>();
  Engine::add_module<ScreenSpaceMeshRenderer>();
  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });
}