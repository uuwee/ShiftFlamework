#include "screenspace_physics.hpp"

#include "screenspace_transform.hpp"
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

void ScreenSpacePhysics::update() {
  for (auto body_wptr : body_list) {
    if (std::shared_ptr<RigidBody> body = body_wptr.lock()) {
      const auto old_position =
          body->entity->get_component<ScreenSpaceTransform>()->position;

      // gravity
      //if (old_position.internal_data[1] - body->radius > -1.0f)
        body->velocity += body->gravity;

      // collider with wall

      if ((old_position + body->velocity).internal_data[0] + body->radius > 1.0f) {
        body->velocity =
            Math::Vector2f({std::min(0.0f, -body->velocity.internal_data[0] *
                                               body->restitution),
                            body->velocity.internal_data[1]});
      }
      if ((old_position + body->velocity).internal_data[0] - body->radius <
          -1.0f) {
        body->velocity =
            Math::Vector2f({std::max(0.0f, -body->velocity.internal_data[0] *
                                               body->restitution),
                            body->velocity.internal_data[1]});
      }
      if ((old_position + body->velocity).internal_data[1] + body->radius >
          1.0) {
        body->velocity =
            Math::Vector2f({body->velocity.internal_data[0],
                            std::min(0.0f, -body->velocity.internal_data[1] *
                                               body->restitution)});
      }
      if ((old_position + body->velocity).internal_data[1] - body->radius <
          -1.0f) {
        body->velocity =
            Math::Vector2f({body->velocity.internal_data[0],
                            std::max(0.0f, -body->velocity.internal_data[1] *
                                               body->restitution)});
      }

      // update transform
      body->entity->get_component<ScreenSpaceTransform>()->position +=
          body->velocity;
      auto transform = body->entity->get_component<ScreenSpaceTransform>();
      transform->position.internal_data[0] =
          std::min(1.0f - body->radius, transform->position.internal_data[0]);
      transform->position.internal_data[0] =
          std::max(-1.0f + body->radius, transform->position.internal_data[0]);
      transform->position.internal_data[1] =
          std::min(1.0f - body->radius, transform->position.internal_data[1]);
      transform->position.internal_data[1] =
          std::max(-1.0f + body->radius, transform->position.internal_data[1]);

      body->velocity = transform->position - old_position;
    }
  }
}