#include "transform.hpp"

#include "engine.hpp"

using namespace SF;

std::shared_ptr<TransformStore> Transform::get_store() {
  return Engine::get_module<TransformStore>();
}

std::shared_ptr<Transform> TransformStore::create(EntityID id) {
  auto instance = std::make_shared<Transform>();
  instances.insert_or_assign(id, instance);
  return instance;
}

std::shared_ptr<Transform> TransformStore::get(EntityID id) {
  return instances.at(id);
}

void TransformStore::remove(EntityID id) {
  auto removed = instances.at(id);
  instances.erase(id);
}

const Math::Vector3f Transform::get_position() { return position; }

const Math::Vector3f Transform::get_scale() { return scale; }

const Math::Vector3f Transform::get_euler_angle() { return euler_angle; }

void Transform::set_position(Math::Vector3f position) {
  this->position = position;
}

void Transform::set_scale(Math::Vector3f scale) { this->scale = scale; }

void Transform::set_euler_angle(Math::Vector3f euler_angle) {
  this->euler_angle = euler_angle;
}

Math::Matrix4x4f Transform::get_world_matrix() {
  const auto translate_mat = Math::Matrix4x4f({{
      {1.0f, 0.0f, 0.0f, position.x},
      {0.0f, 1.0f, 0.0f, position.y},
      {0.0f, 0.0f, 1.0f, position.z},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto scale_mat = Math::Matrix4x4f({{
      {scale.x, 0.0f, 0.0f, 0.0f},
      {0.0f, scale.y, 0.0f, 0.0f},
      {0.0f, 0.0f, scale.z, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto rotate_x = Math::Matrix4x4f({{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, cos(euler_angle.x), -sin(euler_angle.x), 0.0f},
      {0.0f, sin(euler_angle.x), cos(euler_angle.x), 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto rotate_y = Math::Matrix4x4f({{
      {cos(euler_angle.y), 0.0f, sin(euler_angle.y), 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {-sin(euler_angle.y), 0.0f, cos(euler_angle.y), 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto rotate_z = Math::Matrix4x4f({{
      {cos(euler_angle.z), -sin(euler_angle.z), 0.0f, 0.0f},
      {sin(euler_angle.z), cos(euler_angle.z), 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto world_mat =
      translate_mat * rotate_z * rotate_y * rotate_x * scale_mat;
  return world_mat;
}