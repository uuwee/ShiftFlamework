#pragma once

#include "entity.hpp"

namespace SF {
class Script : public Component {
  friend class ScriptStore;

 public:
  static std::shared_ptr<ScriptStore> get_store();
  Script(){};
  ~Script(){};
  void update();
};

class ScriptStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Script>> instances{};

 public:
  static std::string get_name() { return "ScriptStore"; }
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<Script> create(EntityID id);
  std::shared_ptr<Script> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace ShiftFlamework