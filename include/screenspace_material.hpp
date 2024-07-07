#pragma once

#include "entity.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace SF {
class ScreenSpaceMaterial : public Component {
  friend class ScreenSpaceMaterialStore;

 private:
  friend class ScreenSpaceMeshRenderer;
  Math::Vector2f uv_offset = Math::Vector2f({0, 0});
  Math::Vector2f tile_scale = Math::Vector2f({1, 1});
  const uint8_t *data{};
  uint32_t height = 0;
  uint32_t width = 0;

 public:
  static std::shared_ptr<ScreenSpaceMaterialStore> get_store();
  ScreenSpaceMaterial(){};
  ~ScreenSpaceMaterial(){};

  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t* data);

  const Math::Vector2f get_uv_offset();
  const Math::Vector2f get_tile_scale();
};

class ScreenSpaceMaterialStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<ScreenSpaceMaterial>> instances{};

 public:
  static std::string get_name() { return "ScreenSpaceMaterialStore"; }
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<ScreenSpaceMaterial> create(EntityID id);
  std::shared_ptr<ScreenSpaceMaterial> get(EntityID id);
  void remove(EntityID id);
  int size() { return instances.size(); };
};
}  // namespace ShiftFlamework