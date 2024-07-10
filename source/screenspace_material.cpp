#include "screenspace_material.hpp"

#include <memory>

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"
using namespace SF;

const Math::Vector2f ScreenSpaceMaterial::get_uv_offset() { return uv_offset; }

const Math::Vector2f ScreenSpaceMaterial::get_tile_scale() { return tile_scale; }

std::shared_ptr<ScreenSpaceMaterialStore> ScreenSpaceMaterial::get_store() {
  return Engine::get_module<ScreenSpaceMaterialStore>();
}

void ScreenSpaceMaterial::create_gpu_buffer(uint32_t height, uint32_t width,
                                 const uint8_t* data) {
  this->height = height;
  this->width = width;
  this->data = data;

  // notify the renderer that a new material buffer must be created
  Engine::get_module<ScreenSpaceMeshRenderer>()->remove_material_buffer(
      entity_id);
}

std::shared_ptr<SF::ScreenSpaceMaterial> SF::ScreenSpaceMaterialStore::create(
    EntityID id) {
  auto instance = std::make_shared<ScreenSpaceMaterial>();
  instance->entity_id = id;
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<SF::ScreenSpaceMaterial> SF::ScreenSpaceMaterialStore::get(
    EntityID id) {
  return instances.at(id);
}

void SF::ScreenSpaceMaterialStore::remove(EntityID id) {
  auto removed = instances.at(id);
  Engine::get_module<ScreenSpaceMeshRenderer>()->remove_material_buffer(id);

  instances.erase(id);
}
