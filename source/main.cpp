#include <iostream>
#include <memory>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "screenspace_mesh.hpp"
#include "script.hpp"
#include "test_image.h"

using namespace ShiftFlamework;

// flappy bird
std::shared_ptr<Entity> player;
std::vector<std::shared_ptr<Entity>> pipes{};
float player_a = 0;

void main_loop() {
  // user script
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::W) ==
      ButtonState::DOWN) {
    player_a = 0.1f;
  } else {
    player_a -= 0.01f;
  }
  player->get_component<ScreenSpaceTransform>()->position =
      player->get_component<ScreenSpaceTransform>()->position +
      Math::Vector2f({0, player_a});
  float current_y =
      player->get_component<ScreenSpaceTransform>()->position.internal_data[1];
  player->get_component<ScreenSpaceTransform>()->position.internal_data[1] =
      (current_y <= 1.0f ? current_y : 1.0f) >= -1.0f
          ? (current_y <= 1.0f ? current_y : 1.0f)
          : -1.0f;

  Engine::get_module<Input>()->update();
  Engine::get_module<ScreenSpaceMeshRenderer>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 1080, 1080);
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
  player = std::make_shared<Entity>();
  player->add_component<ScreenSpaceMesh>();
  player->add_component<ScreenSpaceTransform>();
  auto p_transform = player->get_component<ScreenSpaceTransform>();
  p_transform->scale = Math::Vector2f({0.4f, 0.4f});
  p_transform->position = Math::Vector2f({-0.3, 0});

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

// pointer to modules
std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
           std::shared_ptr<Input>, std::shared_ptr<ScreenSpaceMeshRenderer>>
    Engine::modules = std::make_tuple(nullptr, nullptr, nullptr, nullptr);

int main() {
  // initialize modules
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  std::get<std::shared_ptr<Input>>(Engine::modules) = std::make_shared<Input>();
  std::get<std::shared_ptr<ScreenSpaceMeshRenderer>>(Engine::modules) =
      std::make_shared<ScreenSpaceMeshRenderer>();
  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });
}