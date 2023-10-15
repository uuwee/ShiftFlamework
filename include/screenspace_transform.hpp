#pragma once

#include "entity.hpp"
#include "vector.hpp"

namespace ShiftFlamework {
class ScreenSpaceTransform : public Component {
 public:
  ScreenSpaceTransform() : Component() {}
  Math::Vector2f position = Math::Vector2f({0, 0});
  float angle = 0;
  Math::Vector2f scale = Math::Vector2f({0, 0});
};
}  // namespace ShiftFlamework