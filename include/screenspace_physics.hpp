#pragma once
#include <memory>
#include <vector>

#include "rigid_body.hpp"

namespace ShiftFlamework {
class ScreenSpacePhysics {
 private:
  std::vector<std::weak_ptr<RigidBody>> body_list{};

 public:
  void initialize();
  void update();
  void register_body(std::shared_ptr<RigidBody> body) {}
  void unregister_body(std::shared_ptr<RigidBody> body) {}
};
}  // namespace ShiftFlamework