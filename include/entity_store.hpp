#pragma once
#include "entity.hpp"

namespace ShiftFlamework {
class EntityStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Entity>> instances;
  int entity_count = 0;

 public:
  void initialize() {
    instances = {};
    instances.clear();
  }

  std::shared_ptr<Entity> create() {
    auto e = std::make_shared<Entity>();
    auto id = entity_count++;
    e->id = id;
    instances.insert_or_assign(id, e);
    return e;
  }
  std::shared_ptr<Entity> get(EntityID id) { return instances[id]; }
  void remove(EntityID id) { instances.erase(id); }
};
}  // namespace ShiftFlamework