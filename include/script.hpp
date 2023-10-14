#pragma once
#include <memory>
#include <string>

#if defined(_MSC_VER)
#include <tchar.h>
#include <windows.h>
#endif

#include "entity.hpp"
#include "entity_script.hpp"

typedef EntityScript* (*action)(void);

namespace ShiftFlamework {
class Script : public Component {
 public:
  Script() : Component() {}
  EntityScript* script = nullptr;
  void load_dll() {
#if defined(_MSC_VER)
    HMODULE module = LoadLibrary(
        "../../../sample/script_import/out/win/Debug/test_script.dll");
    if (module == NULL) return;

    action func1 = (action)GetProcAddress(module, "create_instance");
    script = func1();
    script->on_spawn();
#endif
  }
  void run() { script->on_update(); }
};
}  // namespace ShiftFlamework