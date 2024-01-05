#pragma once
#include <string>
#include <vector>

#include "entity.hpp"
#include "export_object.hpp"
#include "graphics.hpp"
#include "screenspace_transform.hpp"

namespace ShiftFlamework {
struct ScreenSpaceVertex {
 public:
  Math::Vector2f position = Math::Vector2f({0, 0});
  Math::Vector2f texture_coord = Math::Vector2f({0, 0});
};

class ScreenSpaceMesh : public Component {
 private:
  std::vector<ScreenSpaceVertex> vertices;
  std::vector<uint32_t> indices;
  wgpu::Buffer vertex_buffer = nullptr;
  wgpu::Buffer index_buffer = nullptr;

 public:
  ScreenSpaceMesh() : Component(){};
  void on_register();
  void create_gpu_buffer();
  const std::vector<ScreenSpaceVertex> get_vertices();
  const std::vector<uint32_t> get_indices();

  const wgpu::Buffer get_vertex_buffer();
  const wgpu::Buffer get_index_buffer();
};
}  // namespace ShiftFlamework

EXPORT void* ShiftFlamework_Entity_get_component_ShiftFlamework_ScreenSpaceMesh(
    void* self);

EXPORT void* ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceMesh(
    void* self);