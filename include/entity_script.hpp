#pragma once

#if defined(_MSC_VER)
#if defined DLL_EXPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#endif

class EntityScript {
 public:
  virtual void on_spawn(void) = 0;
  virtual void on_update(void) = 0;
};

#if defined(_MSC_VER)
DLL EntityScript* create_instance(void);
#endif