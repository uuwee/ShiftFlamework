#include <export_object.hpp>
#include <iostream>
void ShiftFlamework::ExportObject::add_reference() { reference_count++; };

void ShiftFlamework::ExportObject::remove_reference() {
    std::cout << "remove_reference" << std::endl;
  reference_count--;
  if (reference_count <= 0) {
    delete this;
    std::cout  << "delete this" << std::endl;
  }
};
