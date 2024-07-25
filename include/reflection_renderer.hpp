#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "entity.hpp"
#include "gpu_mesh_buffer.hpp"
#include "graphics.hpp"
#include "vector.hpp"

namespace SF {
class ReflectionRenderer {
 private:
  struct GPUTexture {
    wgpu::Texture texture;
    wgpu::Sampler sampler;
    wgpu::TextureView texture_view;
    bool is_transparent;
  };
  struct MeshBuffer {
    wgpu::Buffer vertex_buffer;
    wgpu::Buffer index_buffer;
    wgpu::Buffer transform_buffer;
  };

  struct DiffusePass {
    wgpu::RenderPipeline render_pipeline;
    wgpu::BindGroupLayout mesh_constant_bind_group_layout;
    wgpu::BindGroupLayout camera_constant_bind_group_layout;
    wgpu::BindGroupLayout texture_bind_group_layout;
  };

  struct AABBPass {
    wgpu::RenderPipeline render_pipeline;
    wgpu::BindGroupLayout aabb_bind_group_layout;
    wgpu::BindGroupLayout camera_constant_bind_group_layout;
  };

  struct InstanceData {
    alignas(16) uint64_t mesh_index;
    alignas(16) uint64_t material_index;
    alignas(16) uint64_t vertex_offset;
    alignas(16) uint64_t vertex_size;
    alignas(16) uint64_t index_offset;
    alignas(16) uint64_t index_size;
  };

  // diffuse pass
  DiffusePass diffuse_pass;
  wgpu::TextureView depth_texture_view;
  wgpu::RenderBundle render_bundle;
  wgpu::BindGroup camera_constant_bind_group;

  std::unordered_map<EntityID, MeshBuffer> gpu_resources{};
  std::unordered_map<std::string, GPUTexture> textures{};

  wgpu::RenderBundle create_diffuse_pass_render_bundle(
      std::vector<EntityID> render_list);

  // unified buffer
  wgpu::Buffer unified_vertex_buffer;
  wgpu::Buffer unified_index_buffer;
  wgpu::Buffer instance_data_buffer;
  std::vector<InstanceData> instance_data;

  // camera parameter
  wgpu::Buffer camera_buffer;

  // dummy texture
  wgpu::Texture texture;
  wgpu::Sampler sampler;
  wgpu::TextureView texture_view;
  wgpu::BindGroup texture_bind_group;

  // aabb
  struct GizmoVertex {
    Math::Vector4f position;
    Math::Vector3f color;
  };
  struct AABB {
    alignas(16) Math::Vector3f min;
    alignas(16) Math::Vector3f max;
  };

  // aabb pass
  AABBPass aabb_pass{};
  wgpu::Buffer gizmo_vertex_buffer;
  wgpu::Buffer gizmo_index_buffer;
  wgpu::Buffer gizmo_constant_buffer;
  wgpu::BindGroup gizmo_constant_bind_group;
  wgpu::BindGroup gizmo_camera_bind_group;
  wgpu::RenderBundle aabb_render_bundle;
  int aabb_count = 10;
  bool aabb_initialized = false;

  void dispose_gpu_resource(EntityID id);
  std::tuple<wgpu::Buffer, wgpu::Buffer> create_mesh_buffer(EntityID id);
  wgpu::Buffer create_constant_buffer(EntityID id);
  void init_aabb_data();

 public:
  static std::string get_name() { return "ReflectionRenderer"; }
  void initialize();
  void render(wgpu::TextureView render_target);
  std::pair<std::string, bool> load_texture(std::string name, std::string path);

  void remove_mesh(EntityID id);
  void remove_constant(EntityID id);

  void draw_ray(Math::Vector3f origin, Math::Vector3f direction, float length,
                Math::Vector3f color);

  // camera parameter
  Math::Vector3f camera_position = Math::Vector3f({0, 3, 0});
  Math::Vector3f camera_angle = Math::Vector3f({0, 0, 0});

  bool lock_command = false;
  bool draw_aabb = false;
};
}  // namespace SF