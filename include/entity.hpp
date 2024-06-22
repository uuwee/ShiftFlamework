#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "export_object.hpp"

namespace ShiftFlamework {
class Component;
class Entity;

using EntityID = unsigned int;

class Component : public ExportObject {
 private:

 public:
  Component() : ExportObject(){};
  ~Component(){};
  EntityID entity_id = 0;
  std::shared_ptr<Entity> get_entity();
};

class Entity : public std::enable_shared_from_this<Entity>,
               public ExportObject {
  friend class EntityStore;

 private:
  std::unordered_map<std::string, std::shared_ptr<Component>> components{};
  EntityID id = 0;

 public:
  Entity();
  ~Entity();
  EntityID get_id();

  template <class T>
  std::shared_ptr<T> get_component() {
    auto name = typeid(T).name();
    auto ptr = T::get_store()->get(id);
    return ptr;
  }

  template <class T,
            typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
  std::shared_ptr<T> add_component() {
    auto component = T::get_store()->create(id);
    std::string str = typeid(T).name();
    std::shared_ptr<Component> casted =
        std::static_pointer_cast<Component>(component);
    components.emplace(str, casted);
    return component;
  }
};

class EntityStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Entity>> instances;
  int entity_count = 0;

 public:
  void initialize() {
    instances = {};
    instances.clear();
  }

  std::shared_ptr<Entity> create() {
    auto e = std::make_shared<Entity>();
    auto id = entity_count++;
    e->id = id;
    instances.insert_or_assign(id, e);
    return e;
  }
  std::shared_ptr<Entity> get(EntityID id) { return instances[id]; }
  void remove(EntityID id) { instances.erase(id); }
};
}  // namespace ShiftFlamework