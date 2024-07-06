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
