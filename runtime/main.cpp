#include <windows.h>

#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>

#include "engine.hpp"
using namespace ShiftFlamework;

typedef void (*init_engine_func_type)(void);
typedef Input* (*get_input_func_type)(void);
typedef void (*set_loop_func_type)(func);

get_input_func_type get_input_func;

void main_loop() {
  if (get_input_func()->get_keyboard_state(Keyboard::A) == ButtonState::DOWN) {
    std::cout << "A down" << std::endl;
  }
}

int main() {
  std::cout << "start runtime" << std::endl;

  HMODULE module = LoadLibrary("./game.dll");
  if (module == NULL) return 1;
  auto init_engine_func =
      (init_engine_func_type)GetProcAddress(module, "initialize_engine");
  get_input_func = (get_input_func_type)GetProcAddress(module, "get_module");
  auto set_loop_func = (set_loop_func_type)GetProcAddress(module, "set_loop");

  set_loop_func(main_loop);

  init_engine_func();

  return 0;
}