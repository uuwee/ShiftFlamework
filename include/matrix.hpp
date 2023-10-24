#pragma once

#include <array>
#include <iostream>
#include <ostream>
#include <vector>

namespace ShiftFlamework::Math {

template <typename T, int row, int column>
class Matrix {
 public:
  Matrix(std::array<std::array<T, column>, row> init_value) {
    internal_data = init_value;
  }
  std::array<std::array<T, column>, row> internal_data;
};

template <typename T, int row, int column>
inline Matrix<T, row, column> operator+(const Matrix<T, row, column>& rhs) {
  auto v = rhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) = v.internal_data.at(i).at(j);
    }
  }
  return v;
}

template <typename T, int row, int column>
inline Matrix<T, row, column> operator-(const Matrix<T, row, column>& rhs) {
  auto v = rhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) = -v.internal_data.at(i).at(j);
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<T, row, column> operator+(const Matrix<T, row, column>& lhs,
                                        const U rhs) {
  auto v = lhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) += rhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<U, row, column> operator+(const T lhs,
                                        const Matrix<U, row, column>& rhs) {
  auto v = rhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) += lhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<T, row, column> operator-(const Matrix<T, row, column>& lhs,
                                        const U rhs) {
  auto v = lhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) -= rhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<U, row, column> operator-(const T lhs,
                                        const Matrix<U, row, column>& rhs) {
  auto v = rhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) -= lhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<T, row, column> operator*(const Matrix<T, row, column>& lhs,
                                        const U rhs) {
  auto v = lhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) *= rhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<U, row, column> operator*(const T lhs,
                                        const Matrix<U, row, column>& rhs) {
  auto v = rhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) *= lhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<T, row, column> operator/(const Matrix<T, row, column>& lhs,
                                        const U rhs) {
  auto v = lhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) /= rhs;
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<U, row, column> operator/(const T lhs,
                                        const Matrix<U, row, column>& rhs) {
  auto v = rhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) = lhs / v.internal_data.at(i).at(j);
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<U, row, column> operator+(const Matrix<T, row, column>& lhs,
                                        const Matrix<U, row, column>& rhs) {
  auto v = lhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) += rhs.internal_data.at(i).at(j);
    }
  }
  return v;
}

template <typename T, int row, int column, typename U>
inline Matrix<U, row, column> operator-(const Matrix<T, row, column>& lhs,
                                        const Matrix<U, row, column>& rhs) {
  auto v = lhs;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      v.internal_data.at(i).at(j) -= rhs.internal_data.at(i).at(j);
    }
  }
  return v;
}

template <typename T, int row, int column, int num, typename U>
inline Matrix<U, row, column> operator*(const Matrix<T, row, num>& lhs,
                                        const Matrix<U, num, column>& rhs) {
  auto v = Matrix<T, row, column>{{}};

  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      for (int k = 0; k < num; k++) {
        if (k == 0) {
          v.internal_data.at(i).at(j) = 0;
        }
        v.internal_data.at(i).at(j) +=
            lhs.internal_data.at(i).at(k) * rhs.internal_data.at(k).at(j);
      }
    }
  }
  return v;
}

template <typename T, int size>
inline Matrix<float, size, size> identity_matrixf() {
  auto v = Matrix<float, size, size>{{}};

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j) {
        v.internal_data.at(i).at(j) = 1;
      } else {
        v.internal_data.at(i).at(j) = 0;
      }
    }
  }
  return v;
};

template <typename T, int size>
inline Matrix<double, size, size> identity_matrixd() {
  auto v = Matrix<double, size, size>{{}};

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j) {
        v.internal_data.at(i).at(j) = 1;
      } else {
        v.internal_data.at(i).at(j) = 0;
      }
    }
  }
  return v;
};

template <typename T, int size>
inline Matrix<int, size, size> identity_matrixi() {
  auto v = Matrix<int, size, size>{{}};

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j) {
        v.internal_data.at(i).at(j) = 1;
      } else {
        v.internal_data.at(i).at(j) = 0;
      }
    }
  }
  return v;
};

template <typename T>
inline Matrix<T, 4, 4> inverse_matrix4(const Matrix<T, 4, 4>& mat) {
  auto m = Matrix<T, 4, 4>{{}};
  int dat = mat.internal_data.at(0).at(0) * mat.internal_data.at(1).at(1) *
                mat.internal_data.at(2).at(2) * mat.internal_data.at(3).at(3) +
            mat.internal_data.at(0).at(0) * mat.internal_data.at(1).at(2) *
                mat.internal_data.at(2).at(3) * mat.internal_data.at(3).at(1) +
            mat.internal_data.at(0).at(0) * mat.internal_data.at(1).at(3) *
                mat.internal_data.at(2).at(1) * mat.internal_data.at(3).at(2) -
            mat.internal_data.at(0).at(0) * mat.internal_data.at(1).at(3) *
                mat.internal_data.at(2).at(2) * mat.internal_data.at(3).at(1) -
            mat.internal_data.at(0).at(0) * mat.internal_data.at(1).at(2) *
                mat.internal_data.at(2).at(1) * mat.internal_data.at(3).at(3) -
            mat.internal_data.at(0).at(0) * mat.internal_data.at(1).at(1) *
                mat.internal_data.at(2).at(3) * mat.internal_data.at(3).at(2) -
            mat.internal_data.at(0).at(1) * mat.internal_data.at(1).at(0) *
                mat.internal_data.at(2).at(2) * mat.internal_data.at(3).at(3) -
            mat.internal_data.at(0).at(2) * mat.internal_data.at(1).at(0) *
                mat.internal_data.at(2).at(3) * mat.internal_data.at(3).at(1) -
            mat.internal_data.at(0).at(3) * mat.internal_data.at(1).at(0) *
                mat.internal_data.at(2).at(1) * mat.internal_data.at(3).at(2) +
            mat.internal_data.at(0).at(3) * mat.internal_data.at(1).at(0) *
                mat.internal_data.at(2).at(2) * mat.internal_data.at(3).at(1) +
            mat.internal_data.at(0).at(2) * mat.internal_data.at(1).at(0) *
                mat.internal_data.at(2).at(1) * mat.internal_data.at(3).at(3) +
            mat.internal_data.at(0).at(1) * mat.internal_data.at(1).at(0) *
                mat.internal_data.at(2).at(3) * mat.internal_data.at(3).at(2);

  return m;
}

template <typename T, int row, int column>
inline Matrix<T, column, row> transposed(const Matrix<T, row, column>& mat) {
  Matrix<T, column, row> transposed = Matrix<T, column, row>{{}};
  for (int i = 0; i < row; i++)
    for (int j = 0; j < column; j++) {
      transposed.internal_data.at(j).at(i) = mat.internal_data.at(i).at(j);
    }

  return transposed;
}

template <typename T, int row, int column>
inline void display_matrix(const Matrix<T, row, column>& m) {
  std::cout << "{ ";

  for (int i = 0; i < row; i++) {
    if (i != 0) {
      std::cout << "  ";
    }
    for (int j = 0; j < column; j++) {
      std::cout << m.internal_data.at(i).at(j) << " ";
    }
    if (i != row - 1) {
      std::cout << std::endl;
    }
  }

  std::cout << "}" << std::endl;
}

using Matrix4x4f = Matrix<float, 4, 4>;
using Matrix3x3f = Matrix<float, 3, 3>;
}  // namespace ShiftFlamework::Math
