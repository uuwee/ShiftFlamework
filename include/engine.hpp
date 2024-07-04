#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "module.hpp"

namespace SF {
class Engine {
 private:
  Engine();
  ~Engine();
  static std::unordered_map<std::string, std::shared_ptr<void>> modules;

 public:
  template <Module T>
  static std::shared_ptr<T> get_module() {
    auto type = T::get_name();
    return std::static_pointer_cast<T>(Engine::modules.at(type));
  }

  template <Module T>
  static std::shared_ptr<T> add_module() {
    std::shared_ptr<T> ptr = std::make_shared<T>();
    std::string str = T::get_name();
    modules.emplace(str, ptr);
    return ptr;
  }

  template <Module T>
  static std::shared_ptr<T> add_module(std::shared_ptr<T> ptr) {
    std::string str = T::get_name();
    modules.emplace(str, ptr);
    return ptr;
  }
};
}  // namespace ShiftFlamework