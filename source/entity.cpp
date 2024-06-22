#include "entity.hpp"

std::shared_ptr<ShiftFlamework::Entity>
ShiftFlamework::Component::get_entity() {
  return entity;
}

void ShiftFlamework::Component::set_entity(
    std::shared_ptr<ShiftFlamework::Entity> e) {
  entity = e;
}

ShiftFlamework::Entity::Entity() : ShiftFlamework::ExportObject() {
  components.clear();
  id = entity_count++;
}

ShiftFlamework::Entity::~Entity() {
  for (auto& c : components) {
    c.second->on_unregister();
  }
  components.clear();
}

ShiftFlamework::EntityID ShiftFlamework::Entity::get_id() { return id; }