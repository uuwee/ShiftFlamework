#include "script.hpp"

#include <windows.h>

#include "engine.hpp"

using namespace SF;

void Script::update() {
  auto lib = LoadLibraryA("runtime.dll");
  auto on_update = (void (*)())GetProcAddress(lib, "on_update");
  on_update();
}

std::shared_ptr<ScriptStore> Script::get_store() {
  return Engine::get_module<ScriptStore>();
}

std::shared_ptr<SF::Script> SF::ScriptStore::create(
    EntityID id) {
  auto instance = std::make_shared<Script>();
  instances.insert_or_assign(id, instance);

  auto lib = LoadLibraryA("runtime.dll");
  auto on_start = (void (*)())GetProcAddress(lib, "on_start");
  on_start();

  return instance;
}

std::shared_ptr<SF::Script> SF::ScriptStore::get(
    EntityID id) {
  return instances.at(id);
}

void SF::ScriptStore::remove(EntityID id) {
  auto removed = instances.at(id);

  auto lib = LoadLibraryA("runtime.dll");
  auto on_end = (void (*)())GetProcAddress(lib, "on_end");
  on_end();

  instances.erase(id);
}