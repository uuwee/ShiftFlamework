#pragma once

#include "graphics.hpp"

namespace ShiftFlamework {
struct GPUTransformBuffer {
 public:
  wgpu::Buffer buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
};
}  // namespace ShiftFlamework