#pragma once

#include "entity.hpp"
#include "vector.hpp"

namespace ShiftFlamework {

class Transform : public Component {
 public:
  Transform() : Component() {}
  Math::Vector3f position = Math::Vector3f({0, 0, 0});
  Math::Vector3f scale = Math::Vector3f({1, 1, 1});
  // ToDo Math::Quaternionf rotation;
};
}  // namespace ShiftFlamework