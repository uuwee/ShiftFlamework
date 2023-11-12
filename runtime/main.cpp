#include <windows.h>

#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>

#include "engine.hpp"
using namespace ShiftFlamework;

typedef void (*init_engine_func_type)(void);

int main() {
  std::cout << "start runtime" << std::endl;

  HMODULE module = LoadLibrary("./game.dll");
  if (module == NULL) return 1;
  auto init_engine_func =
      (init_engine_func_type)GetProcAddress(module, "initialize_engine");

  init_engine_func();

  return 0;
}