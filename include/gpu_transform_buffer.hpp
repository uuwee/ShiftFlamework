#pragma once

#include "graphics.hpp"

namespace SF {
struct GPUTransformBuffer {
 public:
  wgpu::Buffer buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
};
}  // namespace ShiftFlamework