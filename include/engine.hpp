#pragma once

#include <memory>

#include "graphics.hpp"
#include "window.hpp"

namespace ShiftFlamework {
class Engine {
 private:
  Engine();

 public:
  static std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>> modules;

  template <typename T>
  [[nodiscard]] static std::shared_ptr<T> GetModule() noexcept {
    return std::get<std::shared_ptr<T>>(Engine::modules);
  }
};
}  // namespace ShiftFlamework