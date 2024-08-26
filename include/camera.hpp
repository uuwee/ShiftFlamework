#pragma once

#include "matrix.hpp"
#include "vector.hpp"

namespace SF::Math {
Matrix4x4f create_projection_matrix(float ratio = 1, float focal_length = 2.0f,
                                   float near = 0.01f, float far = 100.0f) {
  auto divides = 1.0f / (focal_length * (far - near));
  auto view_proj_mat = Math::Matrix4x4f({{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, ratio, 0.0f, 0.0f},
    {0.0f, 0.0f, far * divides, -far * near * divides},
    {0.0f, 0.0f, 1.0f / focal_length, 1.0f},
  }});
  return view_proj_mat;
}
}  // namespace SF::Math