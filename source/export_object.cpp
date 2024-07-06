#include "export_object.hpp"

namespace SF {
uint32_t ExportObject::count = 0;
ExportObject::ExportObject(std::shared_ptr<void> object) : object(object) {
  //   std::cout << "mat store count: "
  // << Engine::get_module<MaterialStore>()->size() << std::endl;
  //   std::cout << "on constructed, obj.use_count : " << object.use_count()
  // << std::endl;

  count++;
}

ExportObject::~ExportObject() {
  count--;
  // std::cout << "on destructed, obj.use_count : " << object.use_count()
  //  << " ExportObject count: " << count << std::endl;
}
}  // namespace SF