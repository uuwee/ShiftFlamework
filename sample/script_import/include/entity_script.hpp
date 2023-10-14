#pragma once

#if defined(_MSC_VER)
#define DLL __declspec(dllexport)
#endif

class EntityScript {
 public:
  virtual void on_spawn(void) = 0;
  virtual void on_update(void) = 0;
};

extern "C" DLL EntityScript* create_instance(void);