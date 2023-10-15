#include <iostream>
#include <memory>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "script.hpp"

using namespace ShiftFlamework;

void main_loop() {
  // user script

  Engine::get_module<Input>()->update();
  Engine::get_module<Graphics>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 512, 512);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  Engine::get_module<Window>()->initialize_swap_chain(
      Engine::get_module<Graphics>()->instance,
      Engine::get_module<Graphics>()->device);
  Engine::get_module<Graphics>()->create_render_pipeline();

  // game initialize

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