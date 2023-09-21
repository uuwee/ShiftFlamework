#include <iostream>

#include "entity.hpp"

extern "C" __declspec(dllexport) void on_update() {
  std::cout << "hello! this is custom script" << std::endl;
}