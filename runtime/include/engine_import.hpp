#pragma once
#include <windows.h>

#include <cstdint>
#include <iostream>
#include <memory>

namespace ShiftFlamework {
class ScreenSpaceMesh;
class ScreenSpaceTransform;
class Material {
 public:
  void create_gpu_buffer(uint32_t height, uint32_t width, const uint8_t* data) {
    auto lib = LoadLibraryA("game.dll");
    auto ShiftFlamework_Material_create_gpu_buffer =
        (void (*)(void*, const uint32_t, const uint32_t, const uint8_t*))
            GetProcAddress(lib, "ShiftFlamework_Material_create_gpu_buffer");
    ShiftFlamework_Material_create_gpu_buffer(this, height, width, data);
  }
};
class Entity {
 private:
  Entity() = delete;
  ~Entity() = delete;

 public:
  template <typename ComponentType>
  std::shared_ptr<ComponentType> add_component() {
    return nullptr;
  };
  template <typename ComponentType>
  std::shared_ptr<ComponentType> get_component() {
    return nullptr;
  };
};

template <>
std::shared_ptr<ScreenSpaceTransform> Entity::add_component() {
  auto lib = LoadLibraryA("game.dll");
  auto add_component =
      (void* (*)(void*))GetProcAddress(lib,
                                       "ShiftFlamework_Entity_add_component_"
                                       "ShiftFlamework_ScreenSpaceTransform");
  auto ptr = std::shared_ptr<ScreenSpaceTransform>(
      (ScreenSpaceTransform*)add_component(this),
      [&](ScreenSpaceTransform* ptr) {});
  return ptr;
};

template <>
std::shared_ptr<ScreenSpaceTransform> Entity::get_component() {
  auto lib = LoadLibraryA("game.dll");
  auto get_component =
      (void* (*)(void*))GetProcAddress(lib,
                                       "ShiftFlamework_Entity_get_component_"
                                       "ShiftFlamework_ScreenSpaceTransform");
  auto ptr = std::shared_ptr<ScreenSpaceTransform>(
      (ScreenSpaceTransform*)get_component(this),
      [&](ScreenSpaceTransform* ptr) {});
  return ptr;
};

template <>
std::shared_ptr<ScreenSpaceMesh> Entity::add_component() {
  auto lib = LoadLibraryA("game.dll");
  auto add_component =
      (void* (*)(void*))GetProcAddress(lib,
                                       "ShiftFlamework_Entity_add_component_"
                                       "ShiftFlamework_ScreenSpaceMesh");
  auto ptr = std::shared_ptr<ScreenSpaceMesh>(
      (ScreenSpaceMesh*)add_component(this), [&](ScreenSpaceMesh* ptr) {});
  return ptr;
};

template <>
std::shared_ptr<ScreenSpaceMesh> Entity::get_component() {
  auto lib = LoadLibraryA("game.dll");
  auto get_component =
      (void* (*)(void*))GetProcAddress(lib,
                                       "ShiftFlamework_Entity_get_component_"
                                       "ShiftFlamework_ScreenSpaceMesh");
  auto ptr = std::shared_ptr<ScreenSpaceMesh>(
      (ScreenSpaceMesh*)get_component(this), [&](ScreenSpaceMesh* ptr) {});
  return ptr;
};

template <>
std::shared_ptr<Material> Entity::get_component() {
  auto lib = LoadLibraryA("game.dll");
  auto get_component =
      (void* (*)(void*))GetProcAddress(lib,
                                       "ShiftFlamework_Entity_get_component_"
                                       "ShiftFlamework_Material");
  auto ptr = std::shared_ptr<Material>((Material*)get_component(this),
                                       [&](Material* ptr) {});
  return ptr;
};

template <>
std::shared_ptr<Material> Entity::add_component() {
  auto lib = LoadLibraryA("game.dll");
  auto add_component =
      (void* (*)(void*))GetProcAddress(lib,
                                       "ShiftFlamework_Entity_add_component_"
                                       "ShiftFlamework_Material");
  auto ptr = std::shared_ptr<Material>((Material*)add_component(this),
                                       [&](Material* ptr) {});
  return ptr;
};

void destroy_entity(std::shared_ptr<Entity> self) {
  auto lib = LoadLibraryA("game.dll");
  auto destructor =
      (void (*)(void*))GetProcAddress(lib, "ShiftFlamework_Entity_Destructor");
  destructor(self.get());
}

std::shared_ptr<Entity> create_entity() {
  auto lib = LoadLibraryA("game.dll");
  auto constructor =
      (Entity * (*)()) GetProcAddress(lib, "ShiftFlamework_Entity_Constructor");
  auto ptr = constructor();
  auto e = std::shared_ptr<Entity>(ptr, [&](Entity* ptr) {
    // we don't have to call restractor here
    // because entity should be removed from scene when it explicitly destroyed
  });
  return e;
}
}  // namespace ShiftFlamework