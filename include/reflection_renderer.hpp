#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "entity.hpp"
#include "gpu_mesh_buffer.hpp"
#include "gpu_transform_buffer.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace SF {
struct RenderShader {
  const wgpu::RenderPipeline render_pipeline;
  const wgpu::PipelineLayout pipeline_layout;
  const std::vector<wgpu::BindGroupLayout> bind_group_layouts;
};

template <typename T>
struct Buffer {
  wgpu::Buffer buffer;
};

template <typename T>
struct BufferView {
  wgpu::BufferView buffer_view;
};

class ReflectionRenderer {
 private:
  void dispose_gpu_resource(EntityID id);
  void init_aabb_data();

 public:
  static std::string get_name() { return "ReflectionRenderer"; }
  void initialize();
  void render(wgpu::TextureView render_target);
  std::pair<std::string, bool> load_texture(std::string name, std::string path);

  void remove_mesh(EntityID id);
  void remove_constant(EntityID id);

  bool lock_command = false;
  bool draw_aabb = false;
};
}  // namespace SF