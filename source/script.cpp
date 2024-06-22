#include "script.hpp"

#include <windows.h>

using namespace ShiftFlamework;

void Script::on_register() {
  auto lib = LoadLibraryA("runtime.dll");
  auto on_start = (void (*)())GetProcAddress(lib, "on_start");
  on_start();
}

void Script::update() {
  auto lib = LoadLibraryA("runtime.dll");
  auto on_update = (void (*)())GetProcAddress(lib, "on_update");
  on_update();
}

void Script::on_unregister() {
  auto lib = LoadLibraryA("runtime.dll");
  auto on_end = (void (*)())GetProcAddress(lib, "on_end");
  on_end();
}

std::shared_ptr<Script> Script::create(EntityID id) {
  auto script = std::make_shared<Script>();
  instances.insert_or_assign(id, script);
  return script;
}

std::shared_ptr<Script> Script::get(EntityID id) { return instances[id]; }

void Script::remove(EntityID id) { instances.erase(id); }
