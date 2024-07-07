#pragma once

#include <stdint.h>

#include "entity.hpp"

namespace SF {

using MaterialID = uint32_t;

class Material : public Component {
  friend class MaterialStore;

 private:
  MaterialID id = 0;

 public:
  static std::shared_ptr<MaterialStore> get_store();
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