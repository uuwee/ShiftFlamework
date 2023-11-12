#pragma once

#include <memory>
#include <string>

#include "graphics.hpp"
#include "input.hpp"
#include "module.hpp"
#include "screenspace_mesh_renderer.hpp"
#include "screenspace_physics.hpp"
#include "window.hpp"
namespace ShiftFlamework {
class Engine {
 private:
  Engine();

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

}  // namespace ShiftFlamework
extern "C" DLL void initialize_engine(void);

extern "C" DLL ShiftFlamework::Input* get_module();

typedef void (*func)(void);
extern "C" DLL void set_loop(func func);