#pragma once
#include "entity_script.hpp"

class TestScript : public EntityScript {
 public:
  TestScript(){};
  ~TestScript(){};
  void on_update(void);
  void on_spawn(void);
};
