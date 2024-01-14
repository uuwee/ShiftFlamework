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
}

ShiftFlamework::Entity::~Entity() { components.clear(); }
