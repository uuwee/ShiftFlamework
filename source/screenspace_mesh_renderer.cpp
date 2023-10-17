#include "screenspace_mesh_renderer.hpp"

#include "graphics.hpp"
#include "matrix.hpp"

using namespace ShiftFlamework;

uint32_t ceil_to_next_multiple(uint32_t value, uint32_t step) {
  uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
  return step * divide_and_ceil;
}

void ScreenSpaceMeshRenderer::initialize() {
  auto graphics = Engine::get_module<Graphics>();
  auto bind_stride =
      ceil_to_next_multiple(sizeof(Math::Matrix4x4f),
                            graphics->limits.minUniformBufferOffsetAlignment);

  std::vector<wgpu::VertexAttribute> vertex_attributes(2);
  vertex_attributes.at(0).format = wgpu::VertexFormat::Float32x2;
  vertex_attributes.at(0).offset = 0;
  vertex_attributes.at(0).shaderLocation = 0;

  vertex_attributes.at(1).format = wgpu::VertexFormat::Float32x2;
  vertex_attributes.at(1).offset = 2 * 4;  // 2 * sizeof(float32)
  vertex_attributes.at(1).shaderLocation = 1;

  wgpu::VertexBufferLayout vertex_buffer_layout{
      .arrayStride = (2 + 2) * 4,  //(2 + 2) * sizeof(float32)
      .stepMode = wgpu::VertexStepMode::Vertex,
      .attributeCount = static_cast<uint32_t>(vertex_attributes.size()),
      .attributes = vertex_attributes.data(),
  };

  wgpu::BindGroupLayoutEntry binding_layout_entry{
      .binding = 0,
      .visibility = wgpu::ShaderStage::Vertex,
      .buffer = wgpu::BufferBindingLayout{
          .type = wgpu::BufferBindingType::Uniform,
          .hasDynamicOffset = true,
          .minBindingSize = sizeof(Math::Matrix3x3f),
      }};

  wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
      .entryCount = 1, .entries = &binding_layout_entry};

  wgpu::BindGroupLayout bind_group_layout =
      graphics->device.CreateBindGroupLayout(&bind_group_layout_desc);

  wgpu::PipelineLayoutDescriptor layout_desc{
      .bindGroupLayoutCount = 1, .bindGroupLayouts = &bind_group_layout};

  wgpu::PipelineLayout pipeline_layout =
      graphics->device.CreatePipelineLayout(&layout_desc);

  wgpu::BindGroupEntry binding { .binding = 0, .buffer = }
}

void ScreenSpaceMeshRenderer::update() {}