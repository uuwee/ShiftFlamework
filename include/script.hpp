#pragma once

#include "entity.hpp"

namespace ShiftFlamework {
class ScriptStore;
class Script : public Component {
  friend class ScriptStore;

 private:
  void on_register();
  void on_unregister();

 public:
  static std::shared_ptr<ScriptStore> get_store();
  Script(){};
  ~Script(){};
  void update();
};
}  // namespace ShiftFlamework