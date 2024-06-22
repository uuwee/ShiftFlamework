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
  friend class ScreenSpaceMeshStore;

 private:
  std::vector<ScreenSpaceVertex> vertices;
  std::vector<uint32_t> indices;

 public:
  static std::shared_ptr<ScreenSpaceMeshStore> get_store();
  ScreenSpaceMesh(){};
  ~ScreenSpaceMesh(){};

  const std::vector<ScreenSpaceVertex> get_vertices();
  const std::vector<uint32_t> get_indices();
};

class ScreenSpaceMeshStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<ScreenSpaceMesh>> instances{};

 public:
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<ScreenSpaceMesh> create(EntityID id);
  std::shared_ptr<ScreenSpaceMesh> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace ShiftFlamework