#include "rigid_body.hpp"

#include "engine.hpp"
using namespace ShiftFlamework;

RigidBody::RigidBody(float radius = 0.5f,
                     Math::Vector2f velocity = Math::Vector2f({0, 0}))
    : radius(radius),
      velocity(velocity){

      };

void RigidBody::on_register() {
  Engine::get_module<ScreenSpacePhysics>()->register_body(
      entity->get_component<RigidBody>());
};