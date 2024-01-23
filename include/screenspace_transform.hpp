#pragma once

#include "entity.hpp"
#include "export_object.hpp"
#include "graphics.hpp"
#include "vector.hpp"
namespace ShiftFlamework {
class ScreenSpaceTransform : public Component {
 private:
  Math::Vector2f position = Math::Vector2f({0, 0});
  float angle = 0;
  Math::Vector2f scale = Math::Vector2f({1, 1});
  uint32_t z_order = 0;  // min comes foreground

  wgpu::Buffer constant_buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;

 public:
  ~ScreenSpaceTransform(){};
  void create_gpu_buffer();
  void update_gpu_buffer();
  void on_register();

  const Math::Vector2f get_position();
  const float get_angle();
  const Math::Vector2f get_scale();
  const wgpu::Buffer get_constant_buffer();
  const wgpu::BindGroup get_bindgroup();

  void set_position(Math::Vector2f position);
  void set_angle(float angle);
  void set_scale(Math::Vector2f scale);
};
}  // namespace ShiftFlamework
