#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "entity.hpp"
#include "gpu_mesh_buffer.hpp"
#include "gpu_transform_buffer.hpp"
#include "graphics.hpp"

namespace SF {
class ReflectionRenderer {
 private:
  struct GPUResource {
    GPUMeshBuffer mesh_buffer;
    GPUTransformBuffer transform_buffer;
  };

  std::unordered_map<EntityID, GPUResource> gpu_resources{};

  wgpu::BindGroupLayout mesh_constant_bind_group_layout;
  wgpu::BindGroupLayout camera_constant_bind_group_layout;

  wgpu::RenderPipeline render_pipeline;

  wgpu::Buffer camera_buffer;

  wgpu::BindGroup mesh_constant_bind_group;
  wgpu::BindGroup camera_constant_bind_group;

  wgpu::TextureView depthTextureView;

  int count = 0;

  void dispose_gpu_resource(EntityID id);
  GPUMeshBuffer create_mesh_buffer(EntityID id);
  GPUTransformBuffer create_constant_buffer(EntityID id);

 public:
  static std::string get_name() { return "ReflectionRenderer"; }
  void initialize();
  void render(wgpu::TextureView render_target);

  void remove_mesh(EntityID id);
  void remove_constant(EntityID id);
};
}  // namespace SF