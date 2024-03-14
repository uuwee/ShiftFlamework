#include <memory>
#include "include\engine.hpp"
#include "include\entity.hpp"
#include "include\export_object.hpp"
#include "include\graphics.hpp"
#include "include\input.hpp"
#include "include\material.hpp"
#include "include\matrix.hpp"
#include "include\mesh.hpp"
#include "include\physics.hpp"
#include "include\quaternion.hpp"
#include "include\rigid_body.hpp"
#include "include\screenspace_mesh.hpp"
#include "include\screenspace_mesh_renderer.hpp"
#include "include\screenspace_physics.hpp"
#include "include\screenspace_transform.hpp"
#include "include\script.hpp"
#include "include\transform.hpp"
#include "include\vector.hpp"
#include "include\window.hpp"
EXPORT void* ShiftFlamework_Component_Constructor() {
    auto ptr = new ShiftFlamework::Component();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_Component_Destructor(void* self) {
    auto obj = (ShiftFlamework::Component*)self;
    obj->remove_reference();
}

EXPORT std::shared_ptr<Entity> ShiftFlamework_Component_get_entity(void* self) {
    auto obj = (ShiftFlamework::Component*)self;
    obj->get_entity();
}

EXPORT void* ShiftFlamework_Entity_Constructor() {
    auto ptr = new ShiftFlamework::Entity();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_Entity_Destructor(void* self) {
    auto obj = (ShiftFlamework::Entity*)self;
    obj->remove_reference();
}

EXPORT void* ShiftFlamework_Material_Constructor() {
    auto ptr = new ShiftFlamework::Material();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_Material_Destructor(void* self) {
    auto obj = (ShiftFlamework::Material*)self;
    obj->remove_reference();
}

EXPORT void ShiftFlamework_Material_create_gpu_buffer(void* self, uint32_t height, uint32_t width, const uint8_t * data) {
    auto obj = (ShiftFlamework::Material*)self;
    obj->create_gpu_buffer(height, width, data);
}

EXPORT void ShiftFlamework_Material_update_texture_sampling(void* self) {
    auto obj = (ShiftFlamework::Material*)self;
    obj->update_texture_sampling();
}

EXPORT int ShiftFlamework_Material_get_bindgroup(void* self) {
    auto obj = (ShiftFlamework::Material*)self;
    obj->get_bindgroup();
}

EXPORT void ShiftFlamework_Material_on_unregister(void* self) {
    auto obj = (ShiftFlamework::Material*)self;
    obj->on_unregister();
}

EXPORT void* ShiftFlamework_RigidBody_Constructor() {
    auto ptr = new ShiftFlamework::RigidBody();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_RigidBody_Destructor(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->remove_reference();
}

EXPORT void ShiftFlamework_RigidBody_on_register(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->on_register();
}

EXPORT void ShiftFlamework_RigidBody_radius_Setter(void* self, float value) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->radius = value;
}

EXPORT float ShiftFlamework_RigidBody_radius_Getter(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    return obj->radius;
}

EXPORT void ShiftFlamework_RigidBody_restitution_Setter(void* self, float value) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->restitution = value;
}

EXPORT float ShiftFlamework_RigidBody_restitution_Getter(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    return obj->restitution;
}

EXPORT void ShiftFlamework_RigidBody_deceleration_Setter(void* self, float value) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->deceleration = value;
}

EXPORT float ShiftFlamework_RigidBody_deceleration_Getter(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    return obj->deceleration;
}

EXPORT void ShiftFlamework_RigidBody_accel_Setter(void* self, Math::Vector2f value) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->accel = value;
}

EXPORT Math::Vector2f ShiftFlamework_RigidBody_accel_Getter(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    return obj->accel;
}

EXPORT void ShiftFlamework_RigidBody_velocity_Setter(void* self, Math::Vector2f value) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->velocity = value;
}

EXPORT Math::Vector2f ShiftFlamework_RigidBody_velocity_Getter(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    return obj->velocity;
}

EXPORT void ShiftFlamework_RigidBody_gravity_Setter(void* self, Math::Vector2f value) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    obj->gravity = value;
}

EXPORT Math::Vector2f ShiftFlamework_RigidBody_gravity_Getter(void* self) {
    auto obj = (ShiftFlamework::RigidBody*)self;
    return obj->gravity;
}

EXPORT void* ShiftFlamework_ScreenSpaceTransform_Constructor() {
    auto ptr = new ShiftFlamework::ScreenSpaceTransform();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_Destructor(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->remove_reference();
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_create_gpu_buffer(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->create_gpu_buffer();
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_update_gpu_buffer(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->update_gpu_buffer();
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_on_register(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->on_register();
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_on_unregister(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->on_unregister();
}

EXPORT const Math::Vector2f ShiftFlamework_ScreenSpaceTransform_get_position(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->get_position();
}

EXPORT const float ShiftFlamework_ScreenSpaceTransform_get_angle(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->get_angle();
}

EXPORT const Math::Vector2f ShiftFlamework_ScreenSpaceTransform_get_scale(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->get_scale();
}

EXPORT const int ShiftFlamework_ScreenSpaceTransform_get_constant_buffer(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->get_constant_buffer();
}

EXPORT const int ShiftFlamework_ScreenSpaceTransform_get_bindgroup(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->get_bindgroup();
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_set_position(void* self, Math::Vector2f position) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->set_position(position);
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_set_angle(void* self, float angle) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->set_angle(angle);
}

EXPORT void ShiftFlamework_ScreenSpaceTransform_set_scale(void* self, Math::Vector2f scale) {
    auto obj = (ShiftFlamework::ScreenSpaceTransform*)self;
    obj->set_scale(scale);
}

EXPORT void* ShiftFlamework_ScreenSpaceMesh_Constructor() {
    auto ptr = new ShiftFlamework::ScreenSpaceMesh();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_ScreenSpaceMesh_Destructor(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->remove_reference();
}

EXPORT void ShiftFlamework_ScreenSpaceMesh_on_register(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->on_register();
}

EXPORT void ShiftFlamework_ScreenSpaceMesh_on_unregister(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->on_unregister();
}

EXPORT void ShiftFlamework_ScreenSpaceMesh_create_gpu_buffer(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->create_gpu_buffer();
}

EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_vertices(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->get_vertices();
}

EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_indices(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->get_indices();
}

EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_vertex_buffer(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->get_vertex_buffer();
}

EXPORT const int ShiftFlamework_ScreenSpaceMesh_get_index_buffer(void* self) {
    auto obj = (ShiftFlamework::ScreenSpaceMesh*)self;
    obj->get_index_buffer();
}

EXPORT void* ShiftFlamework_Script_Constructor() {
    auto ptr = new ShiftFlamework::Script();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_Script_Destructor(void* self) {
    auto obj = (ShiftFlamework::Script*)self;
    obj->remove_reference();
}

EXPORT void ShiftFlamework_Script_update(void* self) {
    auto obj = (ShiftFlamework::Script*)self;
    obj->update();
}

EXPORT void* ShiftFlamework_Transform_Constructor() {
    auto ptr = new ShiftFlamework::Transform();
    ptr->add_reference();
    return ptr;
}

EXPORT void ShiftFlamework_Transform_Destructor(void* self) {
    auto obj = (ShiftFlamework::Transform*)self;
    obj->remove_reference();
}

EXPORT void ShiftFlamework_Transform_position_Setter(void* self, Math::Vector3f value) {
    auto obj = (ShiftFlamework::Transform*)self;
    obj->position = value;
}

EXPORT Math::Vector3f ShiftFlamework_Transform_position_Getter(void* self) {
    auto obj = (ShiftFlamework::Transform*)self;
    return obj->position;
}

EXPORT void ShiftFlamework_Transform_scale_Setter(void* self, Math::Vector3f value) {
    auto obj = (ShiftFlamework::Transform*)self;
    obj->scale = value;
}

EXPORT Math::Vector3f ShiftFlamework_Transform_scale_Getter(void* self) {
    auto obj = (ShiftFlamework::Transform*)self;
    return obj->scale;
}

