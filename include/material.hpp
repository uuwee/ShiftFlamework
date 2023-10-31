#pragma once

#include "entity.hpp"
#include "graphics.hpp"

namespace ShiftFlamework {
class Material : public Component {
 public:
  wgpu::Texture texture = nullptr;
  wgpu::TextureView texture_view = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
  void create_gpu_buffer();
  void update_gpu_buffer();
};
}  // namespace ShiftFlamework