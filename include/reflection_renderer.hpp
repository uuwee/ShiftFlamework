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
class ReflectionRenderer {
 private:
  struct GPUTexture {
    wgpu::Texture texture;
    wgpu::Sampler sampler;
    wgpu::TextureView texture_view;
    wgpu::BindGroup bind_group;
    bool is_transparent;
  };
  struct GPUResource {
    GPUMeshBuffer mesh_buffer;
    GPUTransformBuffer transform_buffer;
  };

  std::unordered_map<EntityID, GPUResource> gpu_resources{};
  std::unordered_map<std::string, GPUTexture> textures{};

  wgpu::BindGroupLayout mesh_constant_bind_group_layout;
  wgpu::BindGroupLayout camera_constant_bind_group_layout;
  wgpu::BindGroupLayout texture_bind_group_layout;

  wgpu::RenderPipeline render_pipeline;

  Math::Vector3f camera_position = Math::Vector3f({0, 3, 0});
  Math::Vector3f camera_angle = Math::Vector3f({0, 0, 0});
  wgpu::Buffer camera_buffer;

  wgpu::BindGroup camera_constant_bind_group;

  // dummpy texture
  wgpu::Texture texture;
  wgpu::Sampler sampler;
  wgpu::TextureView texture_view;
  wgpu::BindGroup texture_bind_group;

  wgpu::TextureView depthTextureView;

  wgpu::CommandBuffer commands;
  int count = 0;

  void dispose_gpu_resource(EntityID id);
  GPUMeshBuffer create_mesh_buffer(EntityID id);
  GPUTransformBuffer create_constant_buffer(EntityID id);

 public:
  static std::string get_name() { return "ReflectionRenderer"; }
  void initialize();
  void render(wgpu::TextureView render_target);
  std::pair<std::string, bool> load_texture(std::string name, std::string path);

  void remove_mesh(EntityID id);
  void remove_constant(EntityID id);

  bool lock_command = false;
};
}  // namespace SF