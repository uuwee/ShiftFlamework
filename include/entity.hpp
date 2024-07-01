#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <functional>

namespace ShiftFlamework {
class Component;
class Entity;

using EntityID = unsigned int;

class Component{
 private:

 public:
  Component(){};
  ~Component(){};
  EntityID entity_id = 0;
  std::shared_ptr<Entity> get_entity();
};

class Entity : public std::enable_shared_from_this<Entity>{
  friend class EntityStore;

 private:
  std::unordered_map<std::string, std::shared_ptr<void>> components{};
  std::unordered_map<std::string, std::function<void()>> on_destroy_callbacks{};
  EntityID id = 0;

 public:
  Entity();
  ~Entity();
  EntityID get_id();

  template <class T>
  std::shared_ptr<T> get_component() {
    auto name = typeid(T).name();
    if (components.find(name) == components.end()) {
      return nullptr;
    }
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

    on_destroy_callbacks[str] = [this](){
        T::get_store()->remove(id);
    };
    return component;
  }
};

class EntityStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Entity>> instances;
  int entity_count = 0;

 public:
 static std::string get_name() { return "EntityStore"; }
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
  void remove(EntityID id) { 
    for (auto& [key, value] : instances[id]->on_destroy_callbacks){
        value();
    }
    instances.erase(id); 
  }

  std::vector<std::shared_ptr<Entity>> get_all(){
      std::vector<std::shared_ptr<Entity>> vec;
      for(auto& [key, value] : instances){
          vec.push_back(value);
      }
      return vec;
  }
};
}  // namespace ShiftFlamework