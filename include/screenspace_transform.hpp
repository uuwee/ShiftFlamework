#pragma once

#include "entity.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class ScreenSpaceTransform : public Component {
 private:
  uint32_t z_order = 0;  // min comes foreground
  wgpu::Buffer constant_buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;

 public:
  Math::Vector2f position = Math::Vector2f({0, 0});
  float angle = 0;
  Math::Vector2f scale = Math::Vector2f({1, 1});

  ScreenSpaceTransform() : Component() {}
  void create_gpu_buffer();
  void update_gpu_buffer();
  void on_register();

  const wgpu::Buffer get_constant_buffer();
  const wgpu::BindGroup get_bindgroup();
};
}  // namespace ShiftFlamework