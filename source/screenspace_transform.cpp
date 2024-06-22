#include "screenspace_transform.hpp"

#include "engine.hpp"
#include "graphics.hpp"
#include "matrix.hpp"
#include "screenspace_mesh_renderer.hpp"
using namespace ShiftFlamework;

void ScreenSpaceTransform::on_unregister() {
  if (constant_buffer != nullptr) {
    constant_buffer.Destroy();
  }
}

const Math::Vector2f ScreenSpaceTransform::get_position() { return position; }

const float ScreenSpaceTransform::get_angle() { return angle; }

const Math::Vector2f ScreenSpaceTransform::get_scale() { return scale; }

void ScreenSpaceTransform::set_position(Math::Vector2f position) {
  this->position = position;
}

void ScreenSpaceTransform::set_angle(float angle) { this->angle = angle; }

void ScreenSpaceTransform::set_scale(Math::Vector2f scale) {
  this->scale = scale;
}

std::shared_ptr<ScreenSpaceTransformStore> ScreenSpaceTransform::get_store() {
  return Engine::get_module<ScreenSpaceTransformStore>();
}