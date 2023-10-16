#pragma once

#include <stdint.h>

namespace ShiftFlamework {
class ScreenSpaceMeshRenderer {
 public:
  const uint32_t max_instance_count = 1024;
  void initialize();
  void register_mesh();
  void unregister_mesh();
  void update();
};
}  // namespace ShiftFlamework