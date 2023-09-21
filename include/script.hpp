#pragma once
#include <memory>
#include <string>

#if defined(_MSC_VER)
#include <tchar.h>
#include <windows.h>
#endif

#include "entity.hpp"

typedef void (*action)();

namespace ShiftFlamework {
class Script : public Component {
 public:
  Script(std::shared_ptr<Entity> parent) : Component(parent) {}

  void load_dll() {
    HMODULE module = LoadLibrary("test_script.dll");
    if (module == NULL) return;

    action func1 = (action)GetProcAddress(module, "on_update");
    func1();
  }
  void run() {}
};
}  // namespace ShiftFlamework