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
  auto p = ShiftFlamework::Math::Vector3f({1, 0, 0});
  auto rot = ShiftFlamework::Math::angleaxis(std::acos(1 / std::sqrt(2)), ShiftFlamework::Math::Vector3f({0,-1,0}));
  ShiftFlamework::Math::Display_Vector(rot);
  ShiftFlamework::Math::Display_Vector(rot * p);
  rot = ShiftFlamework::Math::euler_yxz(std::acos(1/std::sqrt(2)),0 ,0);
  ShiftFlamework::Math::Display_Vector(rot);
  ShiftFlamework::Math::Display_Vector(rot * p);
  //　テスト終わり
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  Engine::GetModule<Graphics>()->initialize([]() { start(); });
}