#pragma once
#include <windows.h>

#include <memory>

#include "engine.hpp"

#define EXPORT extern "C" __declspec(dllexport)
#define IMPORT extern "C" __declspec(dllimport)

namespace SF {
class ExportObject {
 private:
  static uint32_t count;

 public:
  std::shared_ptr<void> object;
  ExportObject(std::shared_ptr<void> object);
  ~ExportObject();
};
}  // namespace ShiftFlamework