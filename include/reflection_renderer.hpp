#pragma once

#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "entity.hpp"
#include "gpu_mesh_buffer.hpp"
#include "graphics.hpp"
#include "reflection_renderer_pass.hpp"
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

  // diffuse pass
  wgpu::Texture depth_texture;
  DiffusePass diffuse_pass;
  wgpu::RenderBundle render_bundle;
  wgpu::BindGroup camera_constant_bind_group;

  std::unordered_map<EntityID, MeshBuffer> gpu_resources{};
  std::unordered_map<std::filesystem::path, GPUTexture> textures{};

  wgpu::RenderBundle create_diffuse_pass_render_bundle(
      std::vector<EntityID> render_list);

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

  // unified mesh buffer
  wgpu::Buffer unified_vertex_buffer = nullptr;
  wgpu::Buffer unified_index_buffer = nullptr;
  std::vector<InstanceData> instance_data_list;
  wgpu::Buffer instance_data_buffer;
  void update_unified_mesh_buffer();

  // texture pass
  TexturePass texture_pass;
  wgpu::Buffer texture_pass_index_buffer;
  wgpu::BindGroup texture_pass_test_bind_group;

  void dispose_gpu_resource(EntityID id);
  std::tuple<wgpu::Buffer, wgpu::Buffer> create_mesh_buffer(EntityID id);
  wgpu::Buffer create_constant_buffer(EntityID id);
  void init_aabb_data();

  std::mutex texture_load_mutex;

 public:
  static std::string get_name() { return "ReflectionRenderer"; }
  void initialize();
  void render(wgpu::TextureView render_target);
  bool load_texture(std::filesystem::path path);

  void remove_mesh(EntityID id);
  void remove_constant(EntityID id);

  void draw_ray(Math::Vector3f origin, Math::Vector3f direction, float length,
                Math::Vector3f color);

  // camera parameter
  Math::Vector3f camera_position = Math::Vector3f({0, 3, 0});
  Math::Vector3f camera_angle = Math::Vector3f({0, 0, 0});
  bool draw_aabb = false;
};
}  // namespace SF