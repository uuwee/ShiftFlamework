#include <iostream>
#include <memory>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "rigid_body.hpp"
#include "screenspace_mesh.hpp"
#include "script.hpp"
#include "test_image.h"
#include "vector.hpp"
using namespace ShiftFlamework;
std::shared_ptr<Entity> e;
std::shared_ptr<Entity> e1;

void main_loop() {
  // user script
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::Q) ==
      ButtonState::HOLD)
    e->get_component<ScreenSpaceTransform>()->angle += 0.1f;
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::E) ==
      ButtonState::HOLD)
    e->get_component<ScreenSpaceTransform>()->angle -= 0.1f;
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::W) ==
      ButtonState::HOLD) {
    e->get_component<RigidBody>()->velocity.internal_data[1] += 0.015f;
  }

  Engine::get_module<ScreenSpacePhysics>()->update();
  Engine::get_module<Input>()->update();
  Engine::get_module<ScreenSpaceMeshRenderer>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 512, 512);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  wgpu::SupportedLimits supported_limits;
  Engine::get_module<Graphics>()->device.GetLimits(&supported_limits);
  Engine::get_module<Graphics>()->limits = supported_limits.limits;

  Engine::get_module<Window>()->initialize_swap_chain(
      Engine::get_module<Graphics>()->instance,
      Engine::get_module<Graphics>()->device);
  Engine::get_module<ScreenSpaceMeshRenderer>()->initialize();

  // game initialize
  e = std::make_shared<Entity>();
  e->add_component<ScreenSpaceMesh>();
  e->add_component<ScreenSpaceTransform>();
  e->add_component<Material>()->create_gpu_buffer(
      test_image_width, test_image_height, test_image_data);
  e->add_component<RigidBody>()->radius = 0.5f;

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

// pointer to modules
std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
           std::shared_ptr<Input>, std::shared_ptr<ScreenSpaceMeshRenderer>,
           std::shared_ptr<ScreenSpacePhysics>>
    Engine::modules =
        std::make_tuple(nullptr, nullptr, nullptr, nullptr, nullptr);

int main() {
  // initialize modules
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  std::get<std::shared_ptr<Input>>(Engine::modules) = std::make_shared<Input>();
  std::get<std::shared_ptr<ScreenSpaceMeshRenderer>>(Engine::modules) =
      std::make_shared<ScreenSpaceMeshRenderer>();
  std::get<std::shared_ptr<ScreenSpacePhysics>>(Engine::modules) =
      std::make_shared<ScreenSpacePhysics>();
  Engine::get_module<Input>()->initialize();
  Engine::get_module<ScreenSpacePhysics>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });
}