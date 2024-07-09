#include "material.hpp"

#include <memory>

#include "engine.hpp"

using namespace SF;

std::shared_ptr<MaterialStore> Material::get_store() {
  return Engine::get_module<MaterialStore>();
}

std::shared_ptr<Material> MaterialStore::create(EntityID id) {
  auto material = std::make_shared<Material>();
  instances[id] = material;
  return material;
}

std::shared_ptr<Material> MaterialStore::get(EntityID id) {
  return instances[id];
}

void MaterialStore::remove(EntityID id) { instances.erase(id); }
