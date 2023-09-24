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
  Script(std::shared_ptr<Entity> parent) : Component() {}

  void load_dll() {
#if defined(_MSC_VER)
    HMODULE module = LoadLibrary(
        "../../../sample/script_import/out/win/Debug/test_script.dll");
    if (module == NULL) return;

    action func1 = (action)GetProcAddress(module, "on_update");
    func1();
#endif
  }
  void run() {}
};
}  // namespace ShiftFlamework