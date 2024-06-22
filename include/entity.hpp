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
  std::shared_ptr<Entity> entity = nullptr;
  void set_entity(std::shared_ptr<Entity> e);
  friend class Entity;

 public:
  Component() : ExportObject(){};
  ~Component(){};
  std::shared_ptr<Entity> get_entity();
};

class Entity : public std::enable_shared_from_this<Entity>,
               public ExportObject {
 private:
  std::unordered_map<std::string, std::shared_ptr<Component>> components{};
  EntityID id = 0;

 public:
  static int entity_count;  // need to be private, so entity store object should
                            // be there
  Entity();
  ~Entity();
  EntityID get_id();

  template <class T>
  std::shared_ptr<T> get_component() {
    auto name = typeid(T).name();
    auto ptr = T::get(id);
    return ptr;
  }

  template <class T,
            typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
  std::shared_ptr<T> add_component() {
    auto component = T::create(id);
    std::string str = typeid(T).name();
    std::shared_ptr<Component> casted =
        std::static_pointer_cast<Component>(component);
    components.emplace(str, casted);
    component->set_entity(std::shared_ptr<Entity>(this, [&](Entity* ptr) {
      std::cout << "add compoennt delegate" << std::endl;
    }));
    return component;
  }
};
}  // namespace ShiftFlamework