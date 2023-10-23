#pragma once
#include <string>
#include <vector>

#include "entity.hpp"
#include "graphics.hpp"
#include "screenspace_transform.hpp"

namespace ShiftFlamework {
struct ScreenSpaceVertex {
 public:
  Math::Vector2f position = Math::Vector2f({0, 0});
  Math::Vector2f texture_coord = Math::Vector2f({0, 0});
};

class ScreenSpaceMesh : public Component {
 public:
  ScreenSpaceMesh() : Component(){};
  std::vector<ScreenSpaceVertex> vertices;
  std::vector<uint32_t> indices;
  wgpu::Buffer vertex_buffer = nullptr;
  wgpu::Buffer index_buffer = nullptr;

  void on_register();
  void create_gpu_buffer();
};
}  // namespace ShiftFlamework