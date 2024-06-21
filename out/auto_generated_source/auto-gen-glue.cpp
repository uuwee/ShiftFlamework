#include <memory>
#include "include\engine.hpp"
#include "include\entity.hpp"
#include "include\export_object.hpp"
#include "include\graphics.hpp"
#include "include\input.hpp"
#include "include\material.hpp"
#include "include\matrix.hpp"
#include "include\mesh.hpp"
#include "include\quaternion.hpp"
#include "include\screenspace_mesh.hpp"
#include "include\screenspace_mesh_renderer.hpp"
#include "include\screenspace_transform.hpp"
#include "include\script.hpp"
#include "include\transform.hpp"
#include "include\vector.hpp"
#include "include\window.hpp"
#include "out\win\lib\dawn\gen\include\dawn\dawn_proc_table.h"
#include "out\win\lib\dawn\gen\include\dawn\webgpu.h"
#include "out\win\lib\dawn\gen\include\dawn\webgpu_cpp.h"
#include "out\win\lib\dawn\gen\include\dawn\webgpu_cpp_chained_struct.h"
#include "out\win\lib\dawn\gen\include\dawn\webgpu_cpp_print.h"

EXPORT void* ShiftFlamework_Component_Constructor() {
    auto ptr = new ShiftFlamework::Component();
    ptr->add_reference();
    return ptr;
}
                    
EXPORT void ShiftFlamework_Component_Destructor(void* self) {
    auto obj = static_cast<ShiftFlamework::Component*>(self);
    obj->remove_reference();
}
                    
EXPORT std::shared_ptr<ShiftFlamework::Entity> ShiftFlamework_Component_get_entity(void* self) {
    auto obj = static_cast<ShiftFlamework::Component*>(self);
    return obj->get_entity();
}
                    
EXPORT void* ShiftFlamework_Entity_Constructor() {
    auto ptr = new ShiftFlamework::Entity();
    ptr->add_reference();
    return ptr;
}
                    
EXPORT void ShiftFlamework_Entity_Destructor(void* self) {
    auto obj = static_cast<ShiftFlamework::Entity*>(self);
    obj->remove_reference();
}
                    
EXPORT void ShiftFlamework_Material_Destructor(void* self) {
    auto obj = static_cast<ShiftFlamework::Material*>(self);
    obj->remove_reference();
}
                    
EXPORT void ShiftFlamework_Material_create_gpu_buffer(void* self, unsigned int height, unsigned int width, const unsigned char * data) {
    auto obj = static_cast<ShiftFlamework::Material*>(self);
    return obj->create_gpu_buffer(height, width, data);
}
                    
EXPORT void ShiftFlamework_Material_update_texture_sampling(void* self) {
    auto obj = static_cast<ShiftFlamework::Material*>(self);
    return obj->update_texture_sampling();
}
                    
EXPORT int ShiftFlamework_Material_get_bindgroup(void* self) {
    auto obj = static_cast<ShiftFlamework::Material*>(self);
    return obj->get_bindgroup();
}
                    
EXPORT void ShiftFlamework_Material_on_unregister(void* self) {
    auto obj = static_cast<ShiftFlamework::Material*>(self);
    return obj->on_unregister();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_Destructor(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    obj->remove_reference();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_create_gpu_buffer(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->create_gpu_buffer();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_update_gpu_buffer(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->update_gpu_buffer();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_on_register(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->on_register();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_on_unregister(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->on_unregister();
}
                    
EXPORT const ShiftFlamework::Math::Vector<float, 2> ShiftFlamework_ScreenSpaceTransform_get_position(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->get_position();
}
                    
EXPORT const float ShiftFlamework_ScreenSpaceTransform_get_angle(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->get_angle();
}
                    
EXPORT const ShiftFlamework::Math::Vector<float, 2> ShiftFlamework_ScreenSpaceTransform_get_scale(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->get_scale();
}
                    
EXPORT const int ShiftFlamework_ScreenSpaceTransform_get_constant_buffer(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->get_constant_buffer();
}
                    
EXPORT const int ShiftFlamework_ScreenSpaceTransform_get_bindgroup(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->get_bindgroup();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_set_position(void* self, ShiftFlamework::Math::Vector<float, 2> position) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->set_position(position);
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_set_angle(void* self, float angle) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->set_angle(angle);
}
                    
EXPORT void ShiftFlamework_ScreenSpaceTransform_set_scale(void* self, ShiftFlamework::Math::Vector<float, 2> scale) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceTransform*>(self);
    return obj->set_scale(scale);
}
                    
EXPORT void* ShiftFlamework_ScreenSpaceMesh_Constructor() {
    auto ptr = new ShiftFlamework::ScreenSpaceMesh();
    ptr->add_reference();
    return ptr;
}
                    
EXPORT void ShiftFlamework_ScreenSpaceMesh_Destructor(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    obj->remove_reference();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceMesh_on_register(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->on_register();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceMesh_on_unregister(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->on_unregister();
}
                    
EXPORT void ShiftFlamework_ScreenSpaceMesh_create_gpu_buffer(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->create_gpu_buffer();
}
                    
EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_vertices(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->get_vertices();
}
                    
EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_indices(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->get_indices();
}
                    
EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_vertex_buffer(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->get_vertex_buffer();
}
                    
EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_index_buffer(void* self) {
    auto obj = static_cast<ShiftFlamework::ScreenSpaceMesh*>(self);
    return obj->get_index_buffer();
}
                    
EXPORT void ShiftFlamework_Script_update(void* self) {
    auto obj = static_cast<ShiftFlamework::Script*>(self);
    return obj->update();
}
                    
EXPORT void* ShiftFlamework_Transform_Constructor() {
    auto ptr = new ShiftFlamework::Transform();
    ptr->add_reference();
    return ptr;
}
                    