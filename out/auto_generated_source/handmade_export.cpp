#include "engine.hpp"
#include "entity.hpp"
#include "export_object.hpp"
#include "material.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_transform.hpp"
#include "script.hpp"

namespace ShiftFlamework {
// entity.hpp
EXPORT ExportObject* ShiftFlamework_Entity_Constructor() {
  auto ptr = ShiftFlamework::Engine::get_module<ShiftFlamework::EntityStore>()
                 ->create();
  auto container = new ShiftFlamework::ExportObject(ptr);
  return container;
}

EXPORT void ShiftFlamework_Entity_Destructor(ExportObject* self) {
  std::cout << "destructor" << std::endl;
  std::cout << "self: " << self << std::endl;
  Engine::get_module<EntityStore>()->remove(std::static_pointer_cast<Entity>(self->object)->get_id());
  delete self;
}

// screenspace_mesh.hpp
EXPORT ExportObject* ShiftFlamework_Entity_get_component_ShiftFlamework_ScreenSpaceMesh(
    ExportObject* self) {
  auto component = std::static_pointer_cast<Entity>(self->object)->get_component<ScreenSpaceMesh>();
  
  auto container = new ExportObject(component);
  return container;
}

EXPORT ExportObject* ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceMesh(
    ExportObject* self) {
  auto component = std::static_pointer_cast<Entity>(self->object)->add_component<ScreenSpaceMesh>();
  auto container = new ExportObject(component);
  return container;
}

// screenspace_transform.hpp}

EXPORT ExportObject*
ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceTransform(
    ExportObject* self) {
  auto comopnent = std::static_pointer_cast<Entity>(self->object)->add_component<ScreenSpaceTransform>();
  auto container = new ExportObject(comopnent);
  return container;
}

EXPORT ExportObject*
ShiftFlamework_Entity_get_component_ShiftFlamework_ScreenSpaceTransform(
    ExportObject* self) {
  auto comopnent = std::static_pointer_cast<Entity>(self->object)->get_component<ScreenSpaceTransform>();
  auto container = new ExportObject(comopnent);
  return container;
}
// material.hpp

EXPORT ExportObject* ShiftFlamework_Entity_add_component_ShiftFlamework_Material(
    ExportObject* self) {
  auto component = std::static_pointer_cast<Entity>(self->object)->add_component<Material>();
  auto container = new ExportObject(component);
  return container;
}

EXPORT ExportObject* ShiftFlamework_Entity_get_component_ShiftFlamework_Material(
    ExportObject* self) {
  auto component = std::static_pointer_cast<Entity>(self->object)->get_component<Material>();
  auto container = new ExportObject(component);
  return container;
}

EXPORT void ShiftFlamework_Material_create_gpu_buffer(ExportObject* self,
                                                      const uint32_t height,
                                                      const uint32_t width,
                                                      const uint8_t* data) {
  std::cout << "create gpu buffer" << std::endl;
  std::static_pointer_cast<Material>(self->object)->create_gpu_buffer(height, width, data);
  std::cout << "create gpu buffer done. mat count: " << Engine::get_module<MaterialStore>()->size() << "self state: " << self->object.use_count() << std::endl;
}

// script.hpp
EXPORT ExportObject* ShiftFlamework_Entity_add_component_ShiftFlamework_Script(
    ExportObject* self) {
  auto component = std::static_pointer_cast<Entity>(self->object)->add_component<Script>();
  auto container = new ExportObject(component);
  return container;
}

EXPORT ExportObject* ShiftFlamework_Entity_get_component_ShiftFlamework_Script(
    ExportObject* self) {
  auto component = std::static_pointer_cast<Entity>(self->object)->get_component<Script>();
  auto container = new ExportObject(component);
  return container;
}

EXPORT void ShiftFlamework_delete_ExportObject(ExportObject* self) {
  if (self == nullptr) {std::cout << "self is null" << std::endl;}
  if (self->object == nullptr) {std::cout << "object is null" << std::endl;}
    std::cout << "delete object use count: " << self->object.use_count() << std::endl;
  delete self;
}
}  // namespace ShiftFlamework