#pragma once

#include "vector.hpp"

using namespace ShiftFlamework::Math;

namespace ShiftFlamework {
struct Mesh {
  Vector3f position;
  Vector3f normal;
  Vector4f tangent;
  Vector2f texture_coord0;
};
}  // namespace ShiftFlamework