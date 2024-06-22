#include "engine.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_mesh_renderer.hpp"

using namespace ShiftFlamework;
std::shared_ptr<ScreenSpaceMesh> ScreenSpaceMeshStore::create(EntityID id) {
  auto instance = std::make_shared<ScreenSpaceMesh>();
  instances.insert_or_assign(id, instance);
  instance->entity_id = id;

  instance->vertices = {
      ScreenSpaceVertex{.position = Math::Vector2f({-0.5, -0.5}),
                        .texture_coord = Math::Vector2f({0, 0})},
      ScreenSpaceVertex{.position = Math::Vector2f({+0.5, -0.5}),
                        .texture_coord = Math::Vector2f({1, 0})},
      ScreenSpaceVertex{.position = Math::Vector2f({+0.5, +0.5}),
                        .texture_coord = Math::Vector2f({1, 1})},
      ScreenSpaceVertex{.position = Math::Vector2f({-0.5, +0.5}),
                        .texture_coord = Math::Vector2f({0, 1})},
  };

  instance->indices = {0, 1, 2, 2, 3, 0};

  Engine::get_module<ScreenSpaceMeshRenderer>()->register_mesh(instance);
  return instance;
}

std::shared_ptr<ScreenSpaceMesh> ScreenSpaceMeshStore::get(EntityID id) {
  return instances.at(id);
}

void ScreenSpaceMeshStore::remove(EntityID id) {
  auto removed = instances.at(id);
  Engine::get_module<ScreenSpaceMeshRenderer>()->unregister_mesh(removed);
  instances.erase(id);
}