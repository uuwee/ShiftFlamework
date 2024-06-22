#include "material_store.hpp"

std::shared_ptr<ShiftFlamework::Material> ShiftFlamework::MaterialStore::create(
    EntityID id) {
  auto instance = std::make_shared<Material>();
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<ShiftFlamework::Material> ShiftFlamework::MaterialStore::get(
    EntityID id) {
  return instances.at(id);
}

void ShiftFlamework::MaterialStore::remove(EntityID id) { instances.erase(id); }