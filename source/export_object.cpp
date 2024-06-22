#include <export_object.hpp>
#include <iostream>

#include "engine.hpp"
#include "entity.hpp"

void ShiftFlamework::ExportObject::add_reference() { reference_count++; };

void ShiftFlamework::ExportObject::remove_reference() {
  reference_count--;
  std::cout << "remove_reference rest: " << reference_count << std::endl;
  if (reference_count <= 0) {
    // Engine::get_module<EntityStore>()->remove(id);
    // todo we need to delete this object or memory goes freakingly up
    std::cout << "delete this" << std::endl;
  }
};
