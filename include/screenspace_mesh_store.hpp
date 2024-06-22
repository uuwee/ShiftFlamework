#pragma once

#include "screenspace_mesh.hpp"

namespace ShiftFlamework {
class ScreenSpaceMeshStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<ScreenSpaceMesh>> instances;

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