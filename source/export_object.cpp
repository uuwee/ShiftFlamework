#include <export_object.hpp>

void ShiftFlamework::ExportObject::add_reference() { reference_count++; };

void ShiftFlamework::ExportObject::remove_reference() {
  reference_count--;
  if (reference_count <= 0) {
    delete this;
  }
};
