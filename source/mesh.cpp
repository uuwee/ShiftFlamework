#include "mesh.hpp"

#include <memory>

#include "engine.hpp"

using namespace SF;

const std::vector<Vertex> Mesh::get_vertices() { return vertices; }

const std::vector<uint32_t> Mesh::get_indices() { return indices; }

std::shared_ptr<MeshStore> Mesh::get_store() {
  return Engine::get_module<MeshStore>();
}

std::shared_ptr<Mesh> MeshStore::create(EntityID id) {
  auto mesh = std::make_shared<Mesh>();
  instances[id] = mesh;
  return mesh;
}

std::shared_ptr<Mesh> MeshStore::get(EntityID id) { return instances[id]; }
