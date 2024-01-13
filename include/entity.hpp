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
class Component : public ExportObject {
 private:
  std::shared_ptr<Entity> entity = nullptr;

 public:
  Component() : ExportObject(){};
  ~Component(){};
  std::shared_ptr<Entity> get_entity();
  void set_entity(std::shared_ptr<Entity> e);
  virtual void on_register(){};
};

class Entity : public std::enable_shared_from_this<Entity>,
               public ExportObject {
 private:
  std::unordered_map<std::string, std::shared_ptr<Component>> components{};

 public:
  Entity();
  ~Entity();

  template <class T>
  std::shared_ptr<T> get_component() {
    auto name = typeid(T).name();
    auto ptr = components.at(name);
    auto r = std::reinterpret_pointer_cast<T>(ptr);
    return r;
  }

  template <class T,
            typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
  std::shared_ptr<T> add_component() {
    auto component = new T();
    component->add_reference();
    std::shared_ptr<T> ptr =
        std::shared_ptr<T>(component, [&](T* ptr) { ptr->remove_reference(); });
    std::string str = typeid(T).name();
    std::shared_ptr<Component> casted =
        std::static_pointer_cast<Component>(ptr);
    components.emplace(str, casted);
    ptr->set_entity(std::shared_ptr<Entity>(this, [&](Entity* ptr) {
      ptr->remove_reference();
      std::cout << "remove ref entity" << std::endl;
    }));
    // ptr->set_entity(weak_from_this().lock());
    ptr->on_register();
    return ptr;
  }
};
}  // namespace ShiftFlamework

EXPORT void* ShiftFlamework_Entity_Constructor();

EXPORT void ShiftFlamework_Entity_Destructor(void* self);