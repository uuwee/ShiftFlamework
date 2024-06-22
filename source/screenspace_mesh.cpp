#include "screenspace_mesh.hpp"

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"

using namespace ShiftFlamework;

const std::vector<ScreenSpaceVertex> ScreenSpaceMesh::get_vertices() {
  return vertices;
}

const std::vector<uint32_t> ScreenSpaceMesh::get_indices() { return indices; }

std::shared_ptr<ScreenSpaceMesh> ScreenSpaceMesh::create(EntityID id) {
  auto mesh = std::make_shared<ScreenSpaceMesh>();
  instances.emplace(id, mesh);

  // initialize
  mesh->vertices = {
      ScreenSpaceVertex{.position = Math::Vector2f({-0.5, -0.5}),
                        .texture_coord = Math::Vector2f({0, 0})},
      ScreenSpaceVertex{.position = Math::Vector2f({+0.5, -0.5}),
                        .texture_coord = Math::Vector2f({1, 0})},
      ScreenSpaceVertex{.position = Math::Vector2f({+0.5, +0.5}),
                        .texture_coord = Math::Vector2f({1, 1})},
      ScreenSpaceVertex{.position = Math::Vector2f({-0.5, +0.5}),
                        .texture_coord = Math::Vector2f({0, 1})},
  };

  mesh->indices = {0, 1, 2, 0, 2, 3};
  Engine::get_module<ScreenSpaceMeshRenderer>()->register_mesh(
      mesh->get_entity()->get_component<ScreenSpaceMesh>());
  return mesh;
}

std::shared_ptr<ScreenSpaceMesh> ScreenSpaceMesh::get(EntityID id) {
  return instances.at(id);
}

void ScreenSpaceMesh::remove(EntityID id) {
  auto removed = instances.at(id);
  removed->indices.clear();
  removed->vertices.clear();

  Engine::get_module<ScreenSpaceMeshRenderer>()->unregister_mesh(
      removed->get_entity()->get_component<ScreenSpaceMesh>());
  instances.erase(id);
}