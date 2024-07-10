#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "entity.hpp"
#include "vector.hpp"

using namespace SF::Math;

namespace SF {
struct Vertex {
  Vector4f position;
  Vector3f normal;
  Vector4f tangent;
  Vector2f texture_coord0;
};

class Mesh : public Component {
  friend class MeshStore;

 private:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

 public:
  static std::shared_ptr<MeshStore> get_store();
  Mesh(){};
  ~Mesh(){};

  void set_vertices(std::vector<Vertex> vertices);
  void set_indices(std::vector<uint32_t> indices);

  const std::vector<Vertex> get_vertices();
  const std::vector<uint32_t> get_indices();
};

class MeshStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Mesh>> instances{};

 public:
  static std::string get_name() { return "MeshStore"; }
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<Mesh> create(EntityID id);
  std::shared_ptr<Mesh> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace SF