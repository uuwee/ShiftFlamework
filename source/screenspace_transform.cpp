#include "screenspace_transform.hpp"

#include "engine.hpp"
#include "graphics.hpp"
#include "matrix.hpp"
#include "screenspace_mesh_renderer.hpp"
using namespace ShiftFlamework;

void ScreenSpaceTransform::create_gpu_buffer() {
  wgpu::BufferDescriptor buffer_desc{
      .nextInChain = nullptr,
      .label = "constant",
      .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
      .size = Engine::get_module<Graphics>()->get_buffer_stride(
          sizeof(Math::Matrix4x4f)),
      .mappedAtCreation = false,
  };

  constant_buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);

  bindgroup =
      Engine::get_module<ScreenSpaceMeshRenderer>()->create_constant_bind_group(
          constant_buffer);
}

void ScreenSpaceTransform::update_gpu_buffer() {
  const auto rotate_mat =
      Math::Matrix4x4f({{{std::cos(angle), -std::sin(angle), 0, 0},
                         {std::sin(angle), std::cos(angle), 0, 0},
                         {0, 0, 1, 0},
                         {0, 0, 0, 1}}});
  const auto translate_mat = Math::Matrix4x4f({{{1, 0, 0, position.x},
                                                {0, 1, 0, position.y},
                                                {0, 0, 1, 0},
                                                {0, 0, 0, 1}}});
  const auto scale_mat = Math::Matrix4x4f(
      {{{scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}});
  const auto matrix = translate_mat * rotate_mat * scale_mat;
  Engine::get_module<Graphics>()->update_buffer(
      constant_buffer, std::vector(1, transposed(matrix)));
}

void ScreenSpaceTransform::on_register() {
  create_gpu_buffer();
  update_gpu_buffer();
}

void ScreenSpaceTransform::on_unregister() {
  if (constant_buffer != nullptr) {
    constant_buffer.Destroy();
  }
}

const wgpu::Buffer ScreenSpaceTransform::get_constant_buffer() {
  return constant_buffer;
}
const wgpu::BindGroup ScreenSpaceTransform::get_bindgroup() {
  return bindgroup;
}
const Math::Vector2f ScreenSpaceTransform::get_position() { return position; }

const float ScreenSpaceTransform::get_angle() { return angle; }

const Math::Vector2f ScreenSpaceTransform::get_scale() { return scale; }

void ScreenSpaceTransform::set_position(Math::Vector2f position) {
  this->position = position;
  update_gpu_buffer();
}

void ScreenSpaceTransform::set_angle(float angle) {
  this->angle = angle;
  update_gpu_buffer();
}

void ScreenSpaceTransform::set_scale(Math::Vector2f scale) {
  this->scale = scale;
  update_gpu_buffer();
}

std::unordered_map<EntityID, std::shared_ptr<ScreenSpaceTransform>>
    ScreenSpaceTransform::instances;

std::shared_ptr<ScreenSpaceTransform> ScreenSpaceTransform::create(
    EntityID id) {
  auto instance = std::make_shared<ScreenSpaceTransform>();
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<ScreenSpaceTransform> ScreenSpaceTransform::get(EntityID id) {
  return instances[id];
}

void ScreenSpaceTransform::remove(EntityID id) { instances.erase(id); }
