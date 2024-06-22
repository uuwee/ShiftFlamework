#pragma once

#include "screenspace_transform.hpp"

namespace ShiftFlamework {
class ScreenSpaceTransformStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<ScreenSpaceTransform>> instances;

 public:
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<ScreenSpaceTransform> create(EntityID id);
  std::shared_ptr<ScreenSpaceTransform> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace ShiftFlamework