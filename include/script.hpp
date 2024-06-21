#pragma once

#include "entity.hpp"

namespace ShiftFlamework {
class Script : public Component {
 private:
  void on_register();
  void on_unregister();
 public:
  void update();
};
}  // namespace ShiftFlamework