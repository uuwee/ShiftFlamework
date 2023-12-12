#pragma once

#include <array>
#include <iostream>
#include <ostream>

#include "vector.hpp"

namespace ShiftFlamework::Physics {

template <typename T>
class Capsule {
 public:
  Capsule(Vector<T, 3> P, T len, T r) {
    T x = P.getvector(0);
    T y = P.getvector(1);
    T z = P.getvector(2);
  }

 private:
  x, y, z, len, r;

 public:
  void setcapsule_center(Vector<T, 3> center);
  void setcapsule_length(T length);
  void setcapsule_radius(T radius);
  Vector<T, 3> getcapsule_center();
  T getcapsule_length();
  T getcapsule_redius();
};

template <typename T>
class Sphere {
 public:
  Sphere(Vector<T, 3> P, T r) {
    T x = P.getvector(0);
    T y = P.getvector(1);
    T z = P.getvector(2);
  }

 private:
  x, y, z, r;

 public:
  void setcapsule_center(Vector<T, 3> center);
  void setcapsule_radius(T radius);
  Vector<T, 3> getcapsule_center();
  T getcapsule_redius();
};

template <typename T>
class Cube {
 public:
  Cube(Vector<T, 3> P, T x_side, T y_side, T z_side) {
    T x = P.getvector(0);
    T y = P.getvector(1);
    T z = P.getvector(2);
  }

 private:
  x, y, z, x_side, y_side, z_side;

 public:
  void setcapsule_center(Vector<T, 3> center);
  void setcapsule_x_side(T x_len);
  void setcapsule_y_side(T y_len);
  void setcapsule_z_side(T z_len);
  Vector<T, 3> getcapsule_center();
  T getcapsule_x_side();
  T getcapsule_y_side();
  T getcapsule_z_side();
};

}  // namespace ShiftFlamework::Physics