#pragma once

#include <stdint.h>

#include <memory>
#include <vector>

#include "screenspace_mesh.hpp"

namespace ShiftFlamework {
class ScreenSpaceMeshRenderer {
 public:
  uint32_t max_instance_count = 1024;

 private:
  std::vector<std::weak_ptr<ScreenSpaceMesh>> mesh_list{};
  wgpu::RenderPipeline render_pipeline = nullptr;
  wgpu::Buffer constant_buffer_heap = nullptr;
  wgpu::BindGroup constant_buffer_bind_group = nullptr;

  wgpu::Texture test_texture;

 public:
  void initialize(uint32_t max_mesh_count = 8);

  void register_mesh(std::shared_ptr<ScreenSpaceMesh> mesh_component) {
    mesh_list.push_back(mesh_component);
  };

  void unregister_mesh(std::shared_ptr<ScreenSpaceMesh> mesh_component) {
    // todo
    // if we have id for each component(entity)
    // we can use hash map and finish this operation O(1)
    auto ptr = std::begin(mesh_list);
    while (ptr->lock() != mesh_component && ptr != std::end(mesh_list)) ptr++;
    mesh_list.erase(ptr);
  };

  void render(wgpu::TextureView render_target);
};
}  // namespace ShiftFlamework