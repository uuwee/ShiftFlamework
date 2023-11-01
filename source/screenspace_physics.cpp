#include "screenspace_physics.hpp"

using namespace ShiftFlamework;

void ScreenSpacePhysics::register_body(std::shared_ptr<RigidBody> body) {
  body_list.push_back(body);
};

void ScreenSpacePhysics::unregister_body(std::shared_ptr<RigidBody> body) {
  auto ptr = std::begin(body_list);
  while (ptr->lock() != body && ptr != std::end(body_list)) ptr++;
  body_list.erase(ptr);
}

void ScreenSpacePhysics::initialize() {}
void ScreenSpacePhysics::update() {}