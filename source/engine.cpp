#include "engine.hpp"

using namespace ShiftFlamework;

func main_loop_func;

void main_loop() {
  // user script
  main_loop_func();

  Engine::get_module<ScreenSpacePhysics>()->update();
  Engine::get_module<Input>()->update();
  Engine::get_module<ScreenSpaceMeshRenderer>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 1080, 1080);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  wgpu::SupportedLimits supported_limits;
  Engine::get_module<Graphics>()->device.GetLimits(&supported_limits);
  Engine::get_module<Graphics>()->limits = supported_limits.limits;

  Engine::get_module<Window>()->initialize_swap_chain(
      Engine::get_module<Graphics>()->instance,
      Engine::get_module<Graphics>()->device);
  Engine::get_module<ScreenSpaceMeshRenderer>()->initialize();

  // game initialize

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

void Engine::initialize() {
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  std::get<std::shared_ptr<Input>>(Engine::modules) = std::make_shared<Input>();
  std::get<std::shared_ptr<ScreenSpaceMeshRenderer>>(Engine::modules) =
      std::make_shared<ScreenSpaceMeshRenderer>();
  std::get<std::shared_ptr<ScreenSpacePhysics>>(Engine::modules) =
      std::make_shared<ScreenSpacePhysics>();
  Engine::get_module<Input>()->initialize();
  Engine::get_module<ScreenSpacePhysics>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });
};

std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
           std::shared_ptr<Input>, std::shared_ptr<ScreenSpaceMeshRenderer>,
           std::shared_ptr<ScreenSpacePhysics>>
    ShiftFlamework::Engine::modules =
        std::make_tuple(nullptr, nullptr, nullptr, nullptr, nullptr);

DLL void initialize_engine(void) { return Engine::initialize(); }

DLL Graphics* get_graphics_module() {
  return Engine::get_module<Graphics>().get();
}
DLL Window* get_window_module() { return Engine::get_module<Window>().get(); }
DLL Input* get_input_module() { return Engine::get_module<Input>().get(); }
DLL ScreenSpaceMeshRenderer* get_screen_space_mesh_renderer_module() {
  return Engine::get_module<ScreenSpaceMeshRenderer>().get();
}
DLL ScreenSpacePhysics* get_screen_space_physics_module() {
  return Engine::get_module<ScreenSpacePhysics>().get();
}

DLL void set_loop(func func) { main_loop_func = func; }