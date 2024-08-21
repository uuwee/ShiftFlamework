#pragma once

#include <stdint.h>
#include <filesystem>
#include <string>

#include "entity.hpp"

namespace SF {

class Material : public Component {
  friend class MaterialStore;

 private:
 public:
  static std::shared_ptr<MaterialStore> get_store();
  std::filesystem::path texture_path = ""; // TODO: this should be asset unique identifier or something
  std::string id = "";
  bool is_transparent = false;
  Material(){};
  ~Material(){};
};

class MaterialStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Material>> instances{};

 public:
  static std::string get_name() { return "MaterialStore"; }
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<Material> create(EntityID id);
  std::shared_ptr<Material> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace SF