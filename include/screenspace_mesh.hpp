#pragma once
#include <vector>

#include "engine.hpp"
#include "entity.hpp"
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
  ScreenSpaceTransform transform;
  std::vector<ScreenSpaceVertex> vertices;
  std::vector<uint32_t> indices;
  wgpu::Buffer vertex_buffer = nullptr;
  wgpu::Buffer index_buffer = nullptr;

  void on_register() { std::cout << "ssmesh add!" << std::endl; }
  void create_gpu_buffer() {
    {
      const wgpu::BufferDescriptor buffer_desc{
          .nextInChain = nullptr,
          .label = "vertex buffer",
          .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
          .size = vertices.size() * sizeof(ScreenSpaceVertex),
          .mappedAtCreation = false};

      auto graphics = Engine::get_module<Graphics>();
      vertex_buffer = graphics->create_buffer(buffer_desc);
      graphics->update_buffer(vertex_buffer, vertices);
    }
    {
      const wgpu::BufferDescriptor buffer_desc{
          .nextInChain = nullptr,
          .label = "index buffer",
          .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
          .size = indices.size() * sizeof(uint16_t),
          .mappedAtCreation = false,
      };
      auto graphics = Engine::get_module<Graphics>();
      index_buffer = graphics->create_buffer(buffer_desc);
      graphics->update_buffer(index_buffer, indices);
    }
  }
};
}  // namespace ShiftFlamework