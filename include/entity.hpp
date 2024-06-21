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
  void set_entity(std::shared_ptr<Entity> e);
  virtual void on_register(){};
  virtual void on_unregister(){};
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
    auto component = std::make_shared<T>();
    std::string str = typeid(T).name();
    std::shared_ptr<Component> casted =
        std::static_pointer_cast<Component>(component);
    components.emplace(str, casted);
    component->set_entity(std::shared_ptr<Entity>(this, [&](Entity* ptr) {
      std::cout << "add compoennt delegate" << std::endl;   
    }));
    std::static_pointer_cast<Component>(component)->on_register();
    return component;
  }
};
}  // namespace ShiftFlamework