#pragma once

#include <memory>

#include "graphics.hpp"
#include "input.hpp"
#include "window.hpp"

namespace ShiftFlamework {
class Engine {
 private:
  Engine();

 public:
  static std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
                    std::shared_ptr<Input>>
      modules;

  template <typename T>
  [[nodiscard]] static std::shared_ptr<T> get_module() noexcept {
    return std::get<std::shared_ptr<T>>(Engine::modules);
  }
};
}  // namespace ShiftFlamework