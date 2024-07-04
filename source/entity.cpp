#include "entity.hpp"
#include "engine.hpp"

std::shared_ptr<SF::Entity> SF::Component::get_entity() {
  return SF::Engine::get_module<SF::EntityStore>()->get(
      entity_id);
}

SF::Entity::Entity(){
  components.clear();
}

SF::Entity::~Entity() {
  for (auto& c : components) {
    // c.second->on_unregister();
  }
  components.clear();
}

SF::EntityID SF::Entity::get_id() { return id; }