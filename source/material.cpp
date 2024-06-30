#include "material.hpp"

#include <memory>

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"
using namespace ShiftFlamework;

const Math::Vector2f Material::get_uv_offset() { return uv_offset; }

const Math::Vector2f Material::get_tile_scale() { return tile_scale; }

std::shared_ptr<MaterialStore> Material::get_store() {
  return Engine::get_module<MaterialStore>();
}

void Material::create_gpu_buffer(uint32_t height, uint32_t width,
                                 const uint8_t* data) {
  Engine::get_module<ScreenSpaceMeshRenderer>()->create_material_buffer(
      Engine::get_module<EntityStore>()->get(entity_id)->get_id(), height, width, data);
}

std::shared_ptr<ShiftFlamework::Material> ShiftFlamework::MaterialStore::create(
    EntityID id) {
  auto instance = std::make_shared<Material>();
  instance->entity_id = id;
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<ShiftFlamework::Material> ShiftFlamework::MaterialStore::get(
    EntityID id) {
  return instances.at(id);
}

void ShiftFlamework::MaterialStore::remove(EntityID id) {
  auto removed = instances.at(id);
  Engine::get_module<ScreenSpaceMeshRenderer>()->remove_material_buffer(id);
  instances.erase(id);
}
