#pragma once

#include <stdint.h>

#include <memory>
#include <vector>

#include "gpu_mesh_buffer.hpp"
#include "screenspace_mesh.hpp"

namespace ShiftFlamework {
class ScreenSpaceMeshRenderer {
 private:
  std::vector<std::shared_ptr<ScreenSpaceMesh>> mesh_list{};
  std::unordered_map<EntityID, std::shared_ptr<GPUMeshBuffer>>
      gpu_mesh_buffers{};

  wgpu::RenderPipeline render_pipeline = nullptr;
  wgpu::Buffer constant_buffer_heap = nullptr;
  wgpu::BindGroup constant_buffer_bind_group = nullptr;
  wgpu::BindGroup texture_bind_group = nullptr;

  wgpu::Texture test_texture;

  wgpu::BindGroupLayout constant_bind_group_layout;
  wgpu::BindGroupLayout texture_bind_group_layout;

 public:
  void initialize();

  void register_mesh(std::shared_ptr<ScreenSpaceMesh> mesh_component);

  void unregister_mesh(std::shared_ptr<ScreenSpaceMesh> mesh_component);

  void render(wgpu::TextureView render_target);

  wgpu::BindGroup create_constant_bind_group(
      const wgpu::Buffer& constant_buffer);

  wgpu::BindGroup create_texture_bind_group(
      const wgpu::TextureView& texture_view, const wgpu::Sampler& sampler,
      const wgpu::Buffer& tex_offset, const wgpu::Buffer& tile_scale);
};
}  // namespace ShiftFlamework