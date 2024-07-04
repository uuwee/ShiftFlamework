#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "gpu_material_buffer.hpp"
#include "gpu_mesh_buffer.hpp"
#include "gpu_transform_buffer.hpp"
#include "screenspace_mesh.hpp"

namespace ShiftFlamework {
class ScreenSpaceMeshRenderer {
 private:
  struct GPUResource {
    GPUMeshBuffer mesh;
    GPUTransformBuffer transform;
    GPUMaterialBuffer material;
  };
  std::unordered_map<EntityID, std::shared_ptr<GPUMeshBuffer>>
      gpu_mesh_buffers{};
  std::unordered_map<EntityID, std::shared_ptr<GPUTransformBuffer>>
      gpu_transform_buffers{};
  std::unordered_map<EntityID, std::shared_ptr<GPUMaterialBuffer>>
      gpu_material_buffers{};
  std::unordered_map<EntityID, GPUResource> gpu_resources{};

  wgpu::RenderPipeline render_pipeline = nullptr;
  wgpu::Buffer constant_buffer_heap = nullptr;
  wgpu::BindGroup constant_buffer_bind_group = nullptr;
  wgpu::BindGroup texture_bind_group = nullptr;

  wgpu::Texture test_texture;

  wgpu::BindGroupLayout constant_bind_group_layout;
  wgpu::BindGroupLayout texture_bind_group_layout;

  wgpu::BindGroup create_constant_bind_group(
      const wgpu::Buffer& constant_buffer);

  void create_constant_buffer(EntityID id);
  void update_constant_buffer(EntityID id);

  void register_mesh(std::shared_ptr<ScreenSpaceMesh> mesh_component);

  void create_material_buffer(EntityID id, uint32_t height, uint32_t width,
                              const uint8_t* data);

 public:
  static std::string get_name() { return "ScreenSpaceMeshRenderer"; }
  void initialize();

  void render(wgpu::TextureView render_target);

  wgpu::BindGroup create_texture_bind_group(
      const wgpu::TextureView& texture_view, const wgpu::Sampler& sampler,
      const wgpu::Buffer& tex_offset, const wgpu::Buffer& tile_scale);

  void unregister_mesh(EntityID id);
  void remove_constant_buffer(EntityID id);
  void remove_material_buffer(EntityID id);
};
}  // namespace ShiftFlamework