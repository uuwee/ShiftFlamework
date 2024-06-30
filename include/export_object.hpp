#pragma once
#include <windows.h>
#include <memory>

#include "engine.hpp"
#include "Material.hpp"

#define EXPORT extern "C" __declspec(dllexport)
#define IMPORT extern "C" __declspec(dllimport)

namespace ShiftFlamework {
class ExportObject {
 private:

  public:
  std::shared_ptr<void> object;
   ExportObject(std::shared_ptr<void> object) : object(object) {
     std::cout << "mat store count: " << Engine::get_module<MaterialStore>()->size() << std::endl;
     std::cout << "on constructed, obj.use_count : " << object.use_count() << std::endl;
   }

  ~ExportObject() { 
    std::cout << "mat store count: " << Engine::get_module<MaterialStore>()->size() << std::endl;
    std::cout << "on destructed, obj.use_count : " << object.use_count() << std::endl;
  }
};
}  // namespace ShiftFlamework