#include "rigid_body.hpp"

using namespace ShiftFlamework;

RigidBody::RigidBody(float mass, float radius) : radius(radius) {
  inertia = 1.0 / 2.0 * mass * radius * radius;
};