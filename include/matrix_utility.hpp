#pragma once

#include "matrix.hpp"
#include "vector.hpp"

namespace SF::Math {
inline Matrix4x4f euler_angle_to_matrix4x4f(const Vector<float, 3>& angle) {
  const auto rotate_x = Math::Matrix4x4f({{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, cos(angle.x), -sin(angle.x), 0.0f},
      {0.0f, sin(angle.x), cos(angle.x), 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto rotate_y = Math::Matrix4x4f({{
      {cos(angle.y), 0.0f, sin(angle.y), 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {-sin(angle.y), 0.0f, cos(angle.y), 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  const auto rotate_z = Math::Matrix4x4f({{
      {cos(angle.z), -sin(angle.z), 0.0f, 0.0f},
      {sin(angle.z), cos(angle.z), 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});

  return rotate_z * rotate_y * rotate_x;
};

inline Matrix4x4f translation_matrix4x4f(const Vector<float, 3>& translation) {
  return Matrix4x4f({{
      {1.0f, 0.0f, 0.0f, translation.x},
      {0.0f, 1.0f, 0.0f, translation.y},
      {0.0f, 0.0f, 1.0f, translation.z},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});
};

inline Matrix4x4f scale_matrix4x4f(const Vector<float, 3>& scale) {
  return Matrix4x4f({{
      {scale.x, 0.0f, 0.0f, 0.0f},
      {0.0f, scale.y, 0.0f, 0.0f},
      {0.0f, 0.0f, scale.z, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});
};

inline Matrix4x4f perspective_projection(float focal_length, float aspect_ratio,
                                         float near, float far) {
  auto divident = 1.0f / (focal_length * (far - near));
  return Matrix4x4f({{
      {1.0f, 0.0f, 0.0f, 0.0},
      {0.0f, aspect_ratio, 0.0f, 0.0f},
      {0.0f, 0.0f, far * divident, -far * near * divident},
      {0.0f, 0.0f, 1.0f / focal_length, 1.0f},
  }});
};

inline Matrix4x4f orthographics_projection(float scale, float aspect_ratio,
                                           float near, float far) {
  return Matrix4x4f({{
      {1.0f / scale, 0.0f, 0.0f, 0.0f},
      {0.0f, aspect_ratio / scale, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f / (far - near), -near / (far - near)},
      {0.0f, 0.0f, 0.0f, 1.0f},
  }});
};
}  // namespace SF::Math