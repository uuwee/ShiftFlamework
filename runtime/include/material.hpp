#pragma once

#include "entity.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class Material : public Component {
 public:
  wgpu::Texture texture = nullptr;
  wgpu::TextureView texture_view = nullptr;
  wgpu::Sampler sampler = nullptr;
  wgpu::Buffer tex_offset_buffer = nullptr;
  wgpu::Buffer tile_scale_buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t* data);
  Math::Vector2f uv_offset = Math::Vector2f({0, 0});
  Math::Vector2f tile_scale = Math::Vector2f({1, 1});
  void update_texture_sampling();
};
}  // namespace ShiftFlamework