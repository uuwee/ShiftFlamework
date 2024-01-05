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

EXPORT void* ShiftFlamework_Entity_Constructor() {
  auto ptr = new ShiftFlamework::Entity();
  ptr->add_reference();
  return ptr;
}

EXPORT void ShiftFlamework_Entity_Destructor(void* self) {
  ((ShiftFlamework::Entity*)self)->remove_reference();
}