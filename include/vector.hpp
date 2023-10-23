#pragma once

#include <array>
#include <iostream>
#include <ostream>
#include <vector>

namespace ShiftFlamework::Math {

template <typename T, int dimension>
class Vector {
 public:
  Vector(std::array<T, dimension> init_value) { internal_data = init_value; }
  std::array<T, dimension> internal_data;
};

using Vector2f = Vector<float, 2>;
using Vector2d = Vector<double, 3>;
using Vector3f = Vector<float, 3>;
using Vector3d = Vector<double, 3>;
using Vector4f = Vector<float, 4>;
using Vector4d = Vector<double, 4>;

template <typename T, int dimension>
inline Vector<T, dimension> operator+(const Vector<T, dimension>& rhs) {
  auto v = rhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) = v.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension>
inline Vector<T, dimension> operator-(const Vector<T, dimension>& rhs) {
  auto v = rhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) = -v.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator+(const Vector<T, dimension>& lhs,
                                      const U rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) += rhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<U, dimension> operator+(const T lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = rhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) += lhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator-(const Vector<T, dimension>& lhs,
                                      const U rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) -= rhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<U, dimension> operator-(const T lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = rhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) -= lhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator*(const Vector<T, dimension>& lhs,
                                      const U rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) *= rhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<U, dimension> operator*(const T lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = rhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) *= lhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator/(const Vector<T, dimension>& lhs,
                                      const U rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) /= rhs;
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<U, dimension> operator/(const T lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = rhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) = lhs / v.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator+(const Vector<T, dimension>& lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) += rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator-(const Vector<T, dimension>& lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) -= rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator*(const Vector<T, dimension>& lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) *= rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator/(const Vector<T, dimension>& lhs,
                                      const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) /= rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator+=(const Vector<T, dimension>& lhs,
                                       const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) += rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator-=(const Vector<T, dimension>& lhs,
                                       const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) -= rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator*=(const Vector<T, dimension>& lhs,
                                       const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) *= rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension, typename U>
inline Vector<T, dimension> operator/=(const Vector<T, dimension>& lhs,
                                       const Vector<U, dimension>& rhs) {
  auto v = lhs;
  for (int i = 0; i < dimension; i++) {
    v.internal_data.at(i) /= rhs.internal_data.at(i);
  }
  return v;
}

template <typename T, int dimension>
inline Vector<T, dimension> GetNormalized(const Vector<T, dimension>& v) {
  return v / Length(v);
}

template <typename T, int dimension>
inline T Dot(const Vector<T, dimension>& lhs, const Vector<T, dimension>& rhs) {
  int val = 0;
  for (int i = 0; i < dimension; i++) {
    val += lhs.internal_data.at(i) * rhs.internal_data.at(i);
  }
  return val;
}

template <typename T>
inline Vector<T, 3> Cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs) {
  return Vector<T, 3>({lhs.internal_data.at(1) * rhs.internal_data.at(2) -
                           lhs.internal_data.at(2) * rhs.internal_data.at(1),
                       lhs.internal_data.at(2) * rhs.internal_data.at(0) -
                           lhs.internal_data.at(0) * rhs.internal_data.at(2),
                       lhs.internal_data.at(0) * rhs.internal_data.at(1) -
                           lhs.internal_data.at(1) * rhs.internal_data.at(0)});
}

template <typename T, int dimension>
inline void display_vector(const Vector<T, dimension>& v) {
  std::cout << "{ ";

  for (int i = 0; i < dimension; i++) {
    std::cout << v.internal_data.at(i) << " ";
  }

  std::cout << "}" << std::endl;
}

/*使わなくなった機能
template<typename T, int dimension>
inline double Vector<T, dimension>::Length()
{
    int val=0;
    for(int i = 0; i < dimension; i++){
        val += internal_data.at(i) * internal_data.at(i);
    }
    return std::sqrt(val);
}

template<typename T, int dimension>
inline Vector<T, dimension> Vector<T, dimension>::GetNormalized()
{
    return *this / this->Length();
}

template<typename T, int dimension>
inline T Vector<T, dimension>::Dot(const Vector<T, dimension>& rhs)
{
    int val=0;
    for(int i = 0; i < dimension; i++){
        val += internal_data.at(i) * rhs.internal_data.at(i);
    }
    return val;
}

template<typename T, int dimension>
inline Vector<T, 3> Vector<T, dimension>::Cross(const Vector<T, 3>& rhs)
{
    return Vector<T, 3>({internal_data.at(1) * rhs.internal_data.at(2) -
internal_data.at(2) * rhs.internal_data.at(1), internal_data.at(2) *
rhs.internal_data.at(0) - internal_data.at(0) * rhs.internal_data.at(2),
internal_data.at(0) * rhs.internal_data.at(1) - internal_data.at(1) *
rhs.internal_data.at(0)});
}

template<typename T, int dimension>
inline double Length(const Vector<T, dimension>& v)
{
    int val=0;
    for(int i = 0; i < dimension; i++){
        val += v.internal_data.at(i) * v.internal_data.at(i);
    }
    return std::sqrt(val);
}

template<typename T, int dimension>
inline void Vector<T, dimension>::Display()
{
    std::cout << "{ ";

    for (int i = 0; i < dimension; i++){
            std::cout <<internal_data.at(i) << " ";
    }

    std::cout << "}" << std::endl;
}
*/

}  // namespace ShiftFlamework::Math