#pragma once

#include "entity.hpp"
#include "matrix.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class RigidBody : public Component {
 public:
  RigidBody(float radius, Math::Vector2f velocity);
  RigidBody() = default;
  float radius;
  float restitution = 0.5;
  float deceleration = 1.0;
  Math::Vector2f accel = Math::Vector2f({0, 0});
  Math::Vector2f velocity = Math::Vector2f({0, 0});
  Math::Vector2f gravity = Math::Vector2f({0.0f, -0.001f});

  void on_register();
};
}  // namespace ShiftFlamework