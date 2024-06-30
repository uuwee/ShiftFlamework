#pragma once

#include "graphics.hpp"

namespace ShiftFlamework {
struct GPUMeshBuffer {
 public:
  wgpu::Buffer vertex_buffer = nullptr;
  wgpu::Buffer index_buffer = nullptr;
};
}