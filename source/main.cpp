#include <iostream>
#include <memory>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "screenspace_mesh.hpp"
#include "script.hpp"

using namespace ShiftFlamework;

std::shared_ptr<Entity> e1;
void main_loop() {
  // user script
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::Q) ==
      ButtonState::HOLD)
    e1->get_component<ScreenSpaceTransform>()->angle += 0.1f;
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::E) ==
      ButtonState::HOLD)
    e1->get_component<ScreenSpaceTransform>()->angle -= 0.1f;

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
  e1 = std::make_shared<Entity>();
  e1->add_component<ScreenSpaceMesh>();
  auto transform = e1->add_component<ScreenSpaceTransform>();

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