#pragma once

#include "entity.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class Material : public Component {
  friend class MaterialStore;

 private:
  friend class ScreenSpaceMeshRenderer;
  Math::Vector2f uv_offset = Math::Vector2f({0, 0});
  Math::Vector2f tile_scale = Math::Vector2f({1, 1});
  const uint8_t *data{};
  uint32_t height = 0;
  uint32_t width = 0;

 public:
  static std::shared_ptr<MaterialStore> get_store();
  Material(){};
  ~Material(){};

  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t* data);

  const Math::Vector2f get_uv_offset();
  const Math::Vector2f get_tile_scale();
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
  int size() { return instances.size(); };
};
}  // namespace ShiftFlamework