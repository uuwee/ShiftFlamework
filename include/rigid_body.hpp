#pragma once

#include "entity.hpp"
#include "matrix.hpp"
namespace ShiftFlamework {
class RigidBody : public Component {
 public:
  RigidBody(float mass, float radius);
  Math::Matrix2x2f rotation;
  float torque;
  float inertia;
  float radius;
};
}  // namespace ShiftFlamework