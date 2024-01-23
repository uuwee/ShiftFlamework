#pragma once
#include <windows.h>
#define EXPORT extern "C" __declspec(dllexport)
#define IMPORT extern "C" __declspec(dllimport)

namespace ShiftFlamework {
class ExportObject {
 private:
  int reference_count = 0;

 protected:
  virtual ~ExportObject() = default;

 public:
  void add_reference();
  void remove_reference();
};
}  // namespace ShiftFlamework