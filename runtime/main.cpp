#include <windows.h>

#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>

#include "engine.hpp"
#include "material.hpp"
#include "test_image.h"
using namespace ShiftFlamework;

typedef void (*init_engine_func_type)(void);

typedef Graphics* (*get_graphics_func_type)(void);
typedef Window* (*get_window_func_type)(void);
typedef Input* (*get_input_func_type)(void);
typedef ScreenSpaceMeshRenderer* (*get_screen_space_mesh_renderer_func_type)(
    void);
typedef ScreenSpacePhysics* (*get_screen_space_physics_func_type)(void);

typedef void (*set_loop_func_type)(func);

get_graphics_func_type get_graphics_func;
get_window_func_type get_window_func;
get_input_func_type get_input_func;
get_screen_space_mesh_renderer_func_type get_screen_space_mesh_renderer_func;
get_screen_space_physics_func_type get_screen_space_physics_func;
bool initialized = false;

void main_loop() {
  if (!initialized) {
    Engine::modules = std::make_tuple(
        std::shared_ptr<Graphics>((Graphics*)get_graphics_func()),
        std::shared_ptr<Window>((Window*)get_window_func()),
        std::shared_ptr<Input>((Input*)get_input_func()),
        std::shared_ptr<ScreenSpaceMeshRenderer>(
            (ScreenSpaceMeshRenderer*)get_screen_space_mesh_renderer_func()),
        std::shared_ptr<ScreenSpacePhysics>(
            (ScreenSpacePhysics*)get_screen_space_physics_func()));

    initialized = true;
  }

  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::A) ==
      ButtonState::DOWN) {
    std::cout << "A down" << std::endl;
  }
}

std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
           std::shared_ptr<Input>, std::shared_ptr<ScreenSpaceMeshRenderer>,
           std::shared_ptr<ScreenSpacePhysics>>
    Engine::modules =
        std::make_tuple(nullptr, nullptr, nullptr, nullptr, nullptr);

int main() {
  std::cout << "start runtime" << std::endl;

  HMODULE module = LoadLibrary("./game.dll");
  if (module == NULL) return 1;
  auto init_engine_func =
      (init_engine_func_type)GetProcAddress(module, "initialize_engine");

  get_graphics_func =
      (get_graphics_func_type)GetProcAddress(module, "get_graphics_module");
  get_window_func =
      (get_window_func_type)GetProcAddress(module, "get_window_module");
  get_input_func =
      (get_input_func_type)GetProcAddress(module, "get_input_module");
  get_screen_space_mesh_renderer_func =
      (get_screen_space_mesh_renderer_func_type)GetProcAddress(
          module, "get_screen_space_mesh_renderer_module");
  get_screen_space_physics_func =
      (get_screen_space_physics_func_type)GetProcAddress(
          module, "get_screen_space_physics_module");

  auto set_loop_func = (set_loop_func_type)GetProcAddress(module, "set_loop");

  set_loop_func(main_loop);

  init_engine_func();

  return 0;
}