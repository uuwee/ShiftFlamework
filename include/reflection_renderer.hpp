#pragma once

#include <string>
#include <vector>

#include "graphics.hpp"
namespace SF {
class ReflectionRenderer {
 private:
  wgpu::BindGroupLayout mesh_constant_bind_group_layout;
  wgpu::BindGroupLayout camera_constant_bind_group_layout;

  wgpu::RenderPipeline render_pipeline;

  wgpu::Buffer mesh_vertex_buffer;
  wgpu::Buffer mesh_index_buffer;
  wgpu::Buffer mesh_constant_buffer;
  wgpu::Buffer camera_buffer;

  wgpu::BindGroup mesh_constant_bind_group;
  wgpu::BindGroup camera_constant_bind_group;

  int count = 0;

 public:
  static std::string get_name() { return "ReflectionRenderer"; }
  void initialize();
  void render(wgpu::TextureView render_target);
};
}  // namespace SF