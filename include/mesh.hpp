#pragma once

#include "vector.hpp"

using namespace SF::Math;

namespace SF {
struct Mesh {
  Vector3f position;
  Vector3f normal;
  Vector4f tangent;
  Vector2f texture_coord0;
};
}  // namespace ShiftFlamework