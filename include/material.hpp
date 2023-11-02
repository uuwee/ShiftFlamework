#pragma once

#include "entity.hpp"
#include "graphics.hpp"

namespace ShiftFlamework {
class Material : public Component {
 public:
  wgpu::Texture texture = nullptr;
  wgpu::TextureView texture_view = nullptr;
  wgpu::Sampler sampler = nullptr;
  wgpu::Buffer tex_offset_buffer = nullptr;
  wgpu::Buffer tile_scale_buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t *data);
};
}  // namespace ShiftFlamework