#include "engine.hpp"
#include "entity.hpp"
#include "export_object.hpp"
#include "material.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_transform.hpp"
#include "script.hpp"

namespace ShiftFlamework {
// entity.hpp
EXPORT void* ShiftFlamework_Entity_Constructor() {
  auto ptr = ShiftFlamework::Engine::get_module<ShiftFlamework::EntityStore>()
                 ->create();
  ptr->add_reference();
  return ptr.get();
}

EXPORT void ShiftFlamework_Entity_Destructor(void* self) {
  std::cout << "destructor" << std::endl;
  std::cout << "self: " << self << std::endl;
  auto entity = (ShiftFlamework::Entity*)self;
  entity->remove_reference();
  // delete entity;
}

// screenspace_mesh.hpp
EXPORT void* ShiftFlamework_Entity_get_component_ShiftFlamework_ScreenSpaceMesh(
    void* self) {
  auto entity = std::shared_ptr<ShiftFlamework::Entity>(
      (ShiftFlamework::Entity*)self,
      [&](ShiftFlamework::Entity* ptr) { ptr->remove_reference(); });
  auto component = entity->add_component<ShiftFlamework::ScreenSpaceMesh>();
  // auto component = ((ShiftFlamework::Entity*)
  // self)->get_component<ScreenSpaceMesh>();

  component->add_reference();
  return component.get();
}

EXPORT void* ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceMesh(
    void* self) {
  auto component = ((ShiftFlamework::Entity*)self)
                       ->add_component<ShiftFlamework::ScreenSpaceMesh>();
  component->add_reference();
  return component.get();
}

// screenspace_transform.hpp}

EXPORT void*
ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceTransform(
    void* self) {
  auto component = ((ShiftFlamework::Entity*)self)
                       ->add_component<ShiftFlamework::ScreenSpaceTransform>();
  component->add_reference();
  return component.get();
}

EXPORT void*
ShiftFlamework_Entity_get_component_ShiftFlamework_ScreenSpaceTransform(
    void* self) {
  auto component = ((ShiftFlamework::Entity*)self)
                       ->get_component<ShiftFlamework::ScreenSpaceTransform>();
  component->add_reference();
  return component.get();
}
// material.hpp

EXPORT void* ShiftFlamework_Entity_add_component_ShiftFlamework_Material(
    void* self) {
  auto component = ((ShiftFlamework::Entity*)self)
                       ->add_component<ShiftFlamework::Material>();
  component->add_reference();
  return component.get();
}

EXPORT void* ShiftFlamework_Entity_get_component_ShiftFlamework_Material(
    void* self) {
  auto component = ((ShiftFlamework::Entity*)self)
                       ->get_component<ShiftFlamework::Material>();
  component->add_reference();
  return component.get();
}

EXPORT void ShiftFlamework_Material_create_gpu_buffer(void* self,
                                                      const uint32_t height,
                                                      const uint32_t width,
                                                      const uint8_t* data) {
  std::cout << "create gpu buffer" << std::endl;
  ((Material*)self)->create_gpu_buffer(height, width, data);
}

// script.hpp
EXPORT void* ShiftFlamework_Entity_add_component_ShiftFlamework_Script(
    void* self) {
  auto component =
      ((ShiftFlamework::Entity*)self)->add_component<ShiftFlamework::Script>();
  component->add_reference();
  return component.get();
}

EXPORT void* ShiftFlamework_Entity_get_component_ShiftFlamework_Script(
    void* self) {
  auto component =
      ((ShiftFlamework::Entity*)self)->get_component<ShiftFlamework::Script>();
  component->add_reference();
  return component.get();
}
}  // namespace ShiftFlamework