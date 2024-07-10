#pragma once

#include "entity.hpp"
#include "matrix.hpp"
#include "vector.hpp"

namespace SF {

class Transform : public Component {
  friend class TransformStore;

 private:
  Math::Vector3f position = Math::Vector3f({0, 0, 0});
  Math::Vector3f scale = Math::Vector3f({1, 1, 1});
  Math::Vector3f euler_angle = Math::Vector3f({0, 0, 0});
  // ToDo Math::Quaternionf rotation;

 public:
  static std::shared_ptr<TransformStore> get_store();
  Transform(){};
  ~Transform(){};

  const Math::Vector3f get_position();
  const Math::Vector3f get_scale();
  const Math::Vector3f get_euler_angle();

  void set_position(Math::Vector3f position);
  void set_scale(Math::Vector3f scale);
  void set_euler_angle(Math::Vector3f euler_angle);

  Math::Matrix4x4f get_world_matrix();
};

class TransformStore {
 private:
  std::unordered_map<EntityID, std::shared_ptr<Transform>> instances{};

 public:
  static std::string get_name() { return "TransformStore"; }
  void initialize() {
    instances = {};
    instances.clear();
  }
  std::shared_ptr<Transform> create(EntityID id);
  std::shared_ptr<Transform> get(EntityID id);
  void remove(EntityID id);
};
}  // namespace SF