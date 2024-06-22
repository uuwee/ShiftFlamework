#pragma once

#include "material.hpp"

namespace ShiftFlamework {
class MaterialStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Material>> instances;

 public:
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<Material> create(EntityID id);
  std::shared_ptr<Material> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace ShiftFlamework