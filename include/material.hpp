#pragma once

#include "entity.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class Material : public Component {
 private:
  wgpu::Texture texture = nullptr;
  wgpu::TextureView texture_view = nullptr;
  wgpu::Sampler sampler = nullptr;
  wgpu::Buffer tex_offset_buffer = nullptr;
  wgpu::Buffer tile_scale_buffer = nullptr;
  wgpu::BindGroup bindgroup = nullptr;
  Math::Vector2f uv_offset = Math::Vector2f({0, 0});
  Math::Vector2f tile_scale = Math::Vector2f({1, 1});

  Material(){};
  ~Material(){};

  static std::unordered_map<EntityID, std::shared_ptr<Material>> instances;

 public:
  static void initialize() {
    instances = {};
    instances.clear();
  }
  static std::shared_ptr<Material> create(EntityID id);
  static std::shared_ptr<Material> get(EntityID id);
  static void remove(EntityID id);

  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t* data);
  void update_texture_sampling();
  wgpu::BindGroup get_bindgroup();
  void on_unregister();
};
}  // namespace ShiftFlamework