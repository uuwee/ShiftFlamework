#pragma once

#include "graphics.hpp"
#include "vector.hpp"
namespace SF {
struct DiffusePass {
  wgpu::RenderPipeline render_pipeline;
  wgpu::BindGroupLayout mesh_constant_bind_group_layout;
  wgpu::BindGroupLayout camera_constant_bind_group_layout;
  wgpu::BindGroupLayout texture_bind_group_layout;
  wgpu::TextureView depth_texture_view;
};

DiffusePass create_diffuse_pass(Graphics& graphics,
                                wgpu::Texture depth_texture);

struct AABB {
  alignas(16) Math::Vector3f min;
  alignas(16) Math::Vector3f max;
};

struct AABBPass {
  wgpu::RenderPipeline render_pipeline;
  wgpu::BindGroupLayout aabb_data_bind_group_layout;
  wgpu::BindGroupLayout camera_constant_bind_group_layout;
};

AABBPass create_aabb_pass(Graphics& graphics);

struct InstanceData {
  alignas(16) uint64_t vertex_offset;
  alignas(16) uint64_t index_offset;
  alignas(16) uint64_t vertex_size;
  alignas(16) uint64_t index_size;
};

struct TexturePass {
  wgpu::RenderPipeline render_pipeline;
  wgpu::BindGroupLayout texture_bind_group_layout;
};

TexturePass create_texture_pass(Graphics& graphics);

struct PrimaryRayPass {
  wgpu::ComputePipeline compute_pipeline;
  wgpu::BindGroupLayout output_bind_group_layout;
  wgpu::BindGroupLayout geometry_bind_group_layout;
  wgpu::BindGroupLayout aabb_bind_group_layout;
  wgpu::BindGroupLayout camera_bind_group_layout;
};

PrimaryRayPass create_primary_ray_pass(Graphics& graphics);
}  // namespace SF