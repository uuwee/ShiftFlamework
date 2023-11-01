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

std::vector<std::shared_ptr<Entity>> pool{};

void spawn_boal()
{
  auto boal = std::make_shared<Entity>();
  boal->add_component<ScreenSpaceMesh>();
  auto transform = boal->add_component<ScreenSpaceTransform>();
  transform->position = Math::Vector2f({0, 0.8});
  transform->scale = Math::Vector2f({0.25, 0.25});
  boal->add_component<Material>()->create_gpu_buffer(
      test_image_width, test_image_height, test_image_data);
  boal->add_component<RigidBody>()->radius =
      boal->get_component<ScreenSpaceTransform>()->scale.internal_data[0] * 0.5f;
  pool.push_back(boal);
}

void main_loop() {
  // user script
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::W) ==
      ButtonState::DOWN) {
    spawn_boal();
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
  spawn_boal();

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