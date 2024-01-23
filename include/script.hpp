#pragma once

#include "entity.hpp"

namespace ShiftFlamework {
class Script : public Component {
 private:
 public:
  void on_register();
  void on_unregister();
  void update();
};
}  // namespace ShiftFlamework