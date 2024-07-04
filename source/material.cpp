#include "material.hpp"

#include <memory>

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"
using namespace SF;

const Math::Vector2f Material::get_uv_offset() { return uv_offset; }

const Math::Vector2f Material::get_tile_scale() { return tile_scale; }

std::shared_ptr<MaterialStore> Material::get_store() {
  return Engine::get_module<MaterialStore>();
}

void Material::create_gpu_buffer(uint32_t height, uint32_t width,
                                 const uint8_t* data) {
  this->height = height;
  this->width = width;
  this->data = data;

  // notify the renderer that a new material buffer must be created
  Engine::get_module<ScreenSpaceMeshRenderer>()->remove_material_buffer(
      entity_id);
}

std::shared_ptr<SF::Material> SF::MaterialStore::create(
    EntityID id) {
  auto instance = std::make_shared<Material>();
  instance->entity_id = id;
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<SF::Material> SF::MaterialStore::get(
    EntityID id) {
  return instances.at(id);
}

void SF::MaterialStore::remove(EntityID id) {
  auto removed = instances.at(id);
  Engine::get_module<ScreenSpaceMeshRenderer>()->remove_material_buffer(id);

  instances.erase(id);
}
