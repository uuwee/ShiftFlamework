#pragma once

#include <array>
#include <iostream>
#include <ostream>

namespace ShiftFlamework::math{

    template<typename T, int dimension>
    struct Vector
    {
        public:
        Vector(std::array<T, dimension> init_value)
        {
            internal_data = init_value;
        }
            std::array<T, dimension> internal_data;
    };

	using Vector3f = Vector<float, 3>;
	using Vector3d = Vector<double, 3>;
	using Vector4f = Vector<float, 4>;
	using Vector4d = Vector<double, 4>;
}