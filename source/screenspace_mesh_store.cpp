#include "screenspace_mesh_store.hpp"

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"

std::shared_ptr<ShiftFlamework::ScreenSpaceMesh>
ShiftFlamework::ScreenSpaceMeshStore::create(EntityID id) {
  auto instance = std::make_shared<ScreenSpaceMesh>();
  instances.insert_or_assign(id, instance);

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

  instance->indices = {0, 1, 2, 0, 2, 3};
  ShiftFlamework::Engine::get_module<ShiftFlamework::ScreenSpaceMeshRenderer>()
      ->register_mesh(instance->get_entity()->get_component<ScreenSpaceMesh>());
  return instance;
}

std::shared_ptr<ShiftFlamework::ScreenSpaceMesh>
ShiftFlamework::ScreenSpaceMeshStore::get(EntityID id) {
  return instances.at(id);
}

void ShiftFlamework::ScreenSpaceMeshStore::remove(EntityID id) {
  auto removed = instances.at(id);
  removed->indices.clear();
  removed->vertices.clear();

  ShiftFlamework::Engine::get_module<ShiftFlamework::ScreenSpaceMeshRenderer>()
      ->unregister_mesh(
          removed->get_entity()->get_component<ScreenSpaceMesh>());
  instances.erase(id);
}