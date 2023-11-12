#pragma once

#include <memory>
#include <string>

#include "module.hpp"

namespace ShiftFlamework {

class Graphics;
class Window;
class Input;
class ScreenSpaceMeshRenderer;
class ScreenSpacePhysics;

class Engine {
 public:
  static std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
                    std::shared_ptr<Input>,
                    std::shared_ptr<ScreenSpaceMeshRenderer>,
                    std::shared_ptr<ScreenSpacePhysics>>
      modules;

  static void initialize();

  template <typename T>
  [[nodiscard]] static std::shared_ptr<T> get_module() noexcept {
    return std::get<std::shared_ptr<T>>(Engine::modules);
  }
};

extern "C" DLL void initialize_engine(void);

extern "C" DLL void* get_module(std::string name);
}  // namespace ShiftFlamework