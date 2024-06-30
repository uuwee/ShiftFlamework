#include "entity.hpp"
#include "engine.hpp"

std::shared_ptr<ShiftFlamework::Entity> ShiftFlamework::Component::get_entity() {
  return ShiftFlamework::Engine::get_module<ShiftFlamework::EntityStore>()->get(
      entity_id);
}

ShiftFlamework::Entity::Entity(){
  components.clear();
}

ShiftFlamework::Entity::~Entity() {
  for (auto& c : components) {
    // c.second->on_unregister();
  }
  components.clear();
}

ShiftFlamework::EntityID ShiftFlamework::Entity::get_id() { return id; }