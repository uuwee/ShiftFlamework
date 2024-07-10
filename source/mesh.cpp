#include "mesh.hpp"

#include <memory>

#include "engine.hpp"

using namespace SF;

const std::vector<Vertex> Mesh::get_vertices() { return vertices; }

const std::vector<uint32_t> Mesh::get_indices() { return indices; }

void Mesh::set_vertices(std::vector<Vertex> vertices) {
  this->vertices = vertices;
}

void Mesh::set_indices(std::vector<uint32_t> indices) {
  this->indices = indices;
}

std::shared_ptr<MeshStore> Mesh::get_store() {
  return Engine::get_module<MeshStore>();
}

std::shared_ptr<Mesh> MeshStore::create(EntityID id) {
  auto mesh = std::make_shared<Mesh>();
  instances[id] = mesh;
  return mesh;
}

std::shared_ptr<Mesh> MeshStore::get(EntityID id) { return instances[id]; }

void MeshStore::remove(EntityID id) { instances.erase(id); }