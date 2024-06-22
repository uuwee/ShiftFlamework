#pragma once

#include "entity.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class MaterialStore;
class Material : public Component {
  friend class MaterialStore;

 private:
  Math::Vector2f uv_offset = Math::Vector2f({0, 0});
  Math::Vector2f tile_scale = Math::Vector2f({1, 1});

 public:
  std::shared_ptr<MaterialStore> get_store();
  Material(){};
  ~Material(){};

  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t* data);
  void update_texture_sampling();

  const Math::Vector2f get_uv_offset();
  const Math::Vector2f get_tile_scale();
};
}  // namespace ShiftFlamework