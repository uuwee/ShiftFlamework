#pragma once
#include "entity.hpp"
#include "screenspace_transform.hpp"

namespace ShiftFlamework {
class ScreenSpaceMesh : public Component {
 public:
  ScreenSpaceMesh() : Component(){};
  ScreenSpaceTransform transform;
}
}  // namespace ShiftFlamework