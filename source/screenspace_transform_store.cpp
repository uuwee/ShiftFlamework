#include "screenspace_transform_store.hpp"

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"

std::shared_ptr<ShiftFlamework::ScreenSpaceTransform>
ShiftFlamework::ScreenSpaceTransformStore::create(EntityID id) {
  auto instance = std::make_shared<ScreenSpaceTransform>();
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<ShiftFlamework::ScreenSpaceTransform>
ShiftFlamework::ScreenSpaceTransformStore::get(EntityID id) {
  return instances.at(id);
}

void ShiftFlamework::ScreenSpaceTransformStore::remove(EntityID id) {
  Engine::get_module<ScreenSpaceMeshRenderer>()->remove_constant_buffer(id);
  auto removed = instances.at(id);
  instances.erase(id);
}