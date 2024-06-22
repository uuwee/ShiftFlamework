#pragma once

#include "entity.hpp"
#include "export_object.hpp"
#include "graphics.hpp"
#include "vector.hpp"
namespace ShiftFlamework {
class ScreenSpaceTransformStore;
class ScreenSpaceTransform : public Component {
  friend class ScreenSpaceTransformStore;

 private:
  Math::Vector2f position = Math::Vector2f({0, 0});
  float angle = 0;
  Math::Vector2f scale = Math::Vector2f({1, 1});
  uint32_t z_order = 0;  // min comes foreground

 public:
  static std::shared_ptr<ScreenSpaceTransformStore> get_store();

  ScreenSpaceTransform(){};
  ~ScreenSpaceTransform(){};

  const Math::Vector2f get_position();
  const float get_angle();
  const Math::Vector2f get_scale();

  void set_position(Math::Vector2f position);
  void set_angle(float angle);
  void set_scale(Math::Vector2f scale);
};
}  // namespace ShiftFlamework
