#pragma once

#include <stdint.h>

#include <memory>
#include <vector>

#include "screenspace_mesh.hpp"

namespace ShiftFlamework {
class ScreenSpaceMeshRenderer {
 public:
  static constexpr size_t max_instance_count = 1024;

 private:
  std::vector<std::weak_ptr<ScreenSpaceMesh>> mesh_list{};
  wgpu::RenderPipeline render_pipeline = nullptr;

 public:
  void initialize();
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
  void update();
};
}  // namespace ShiftFlamework