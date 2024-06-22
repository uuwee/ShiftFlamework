#pragma once

#include "entity.hpp"

namespace ShiftFlamework {
class Script : public Component {
 private:
  void on_register();
  void on_unregister();

  static std::unordered_map<EntityID, std::shared_ptr<Script>> instances;

 public:
  Script(){};
  ~Script(){};
  void update();

  static std::shared_ptr<Script> create(EntityID id);
  static std::shared_ptr<Script> get(EntityID id);
  static void remove(EntityID id);
  static void initialize() {
    instances = {};
    instances.clear();
  }
};
}  // namespace ShiftFlamework