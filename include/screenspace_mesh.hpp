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

  static std::unordered_map<EntityID, std::shared_ptr<ScreenSpaceMesh>>
      instances;

 public:
  ScreenSpaceMesh(){};
  ~ScreenSpaceMesh(){};
  static void initialize() {
    instances = {};
    instances.clear();
  }
  static std::shared_ptr<ScreenSpaceMesh> create(EntityID id);
  static std::shared_ptr<ScreenSpaceMesh> get(EntityID id);
  static void remove(EntityID id);

  void on_register();
  void on_unregister();
  void create_gpu_buffer();
  const std::vector<ScreenSpaceVertex> get_vertices();
  const std::vector<uint32_t> get_indices();
};
}  // namespace ShiftFlamework