#pragma once

#include "graphics.hpp"

namespace SF {
struct DiffusePass{
    wgpu::RenderPipeline render_pipeline;
    wgpu::BindGroupLayout mesh_constant_bind_group_layout;
    wgpu::BindGroupLayout camera_constant_bind_group_layout;
    wgpu::BindGroupLayout texture_bind_group_layout;
    wgpu::TextureView depth_texture_view;
};

DiffusePass create_diffuse_pass(Graphics& graphics, wgpu::Texture depth_texture);

}