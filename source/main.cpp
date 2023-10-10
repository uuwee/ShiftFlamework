#include <iostream>
#include <memory>
#include <tuple>

#include "engine.hpp"
#include "quaternion.hpp"
#include "vector3.hpp"
#include "matrix.hpp"

using namespace ShiftFlamework;

void main_loop() {
  Engine::GetModule<Graphics>()->render(
      Engine::GetModule<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 512, 512);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  Engine::GetModule<Window>()->initialize_swap_chain(
      Engine::GetModule<Graphics>()->instance,
      Engine::GetModule<Graphics>()->device);
  Engine::GetModule<Graphics>()->create_render_pipeline();
  Engine::GetModule<Window>()->start_main_loop(main_loop);
}

std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>> Engine::modules;

int main() {
  // テスト始め

  //　テスト終わり
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  Engine::GetModule<Graphics>()->initialize([]() { start(); });
}