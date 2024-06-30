#pragma once
#include "graphics.hpp"

namespace ShiftFlamework {
struct GPUMaterialBuffer {
 public:
  wgpu::Texture texture = nullptr;
  wgpu::TextureView texture_view = nullptr;
  wgpu::Sampler sampler = nullptr;
  wgpu::Buffer tex_offset_buffer = nullptr;
  wgpu::Buffer tile_scale_buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
};
}  // namespace ShiftFlamework