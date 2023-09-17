#pragma once

#include <array>
#include <iostream>
#include <ostream>
#include<vector>

namespace ShiftFlamework::math{

    template<typename T, int dimension>
    class Vector
    {
        public:
        Vector(std::array<T, dimension> init_value)
        {
            internal_data = init_value;
        }

        private:
        std::array<T, dimension> internal_data;

        public:
        void setVector(int i, T n);
        T getVector(int i) const;

    };

    template<typename T>
    class Vector<T, 3>
    {
        public:
        Vector(std::array<T, 3> init_value)
        {
            x = init_value.at(0);
            y = init_value.at(1);
            z = init_value.at(2);
        }

        void setVector(int i, T n);
        T getVector(int i) const;
        T x;
        T y;
        T z;

    };

	using Vector3f = Vector<float, 3>;
	using Vector3d = Vector<double, 3>;
	using Vector4f = Vector<float, 4>;
	using Vector4d = Vector<double, 4>;

    template<typename T, int dimension>
    inline void Vector<T, dimension>::setVector(int i, T n)
    {
        internal_data.at(i)=n;
    }

    template<typename T>
    inline void Vector<T, 3>::setVector(int i, T n)
    {
        if(i == 0){
            x = n;
        }else if(i == 1){
            y = n;
        }else if(i == 2){
            z = n;
        }
    }

    template<typename T, int dimension>
    inline T Vector<T, dimension>::getVector(int i) const
    {
        return internal_data.at(i);
    }

    template<typename T>
    inline T Vector<T, 3>::getVector(int i) const
    {
        if(i == 0){
            return x;
        }else if(i == 1){
            return y;
        }else if(i == 2){
            return z;
        }
    }

    template<typename T, int dimension>
    inline Vector<T, dimension> operator + (const Vector<T, dimension>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension>
    inline Vector<T, dimension> operator - (const Vector<T, dimension>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < dimension; i++){
            std::cout << i<< " : " << -v.getVector(i) << std::endl;
            v.setVector(i, -v.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator + (const Vector<T, dimension>& lhs, const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) + rhs);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<U, dimension> operator + (const T lhs, const Vector<U, dimension>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) + lhs);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator - (const Vector<T, dimension>& lhs, const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) - rhs);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<U, dimension> operator - (const T lhs, const Vector<U, dimension>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, lhs - v.getVector(i));
        }
        return v;
    }
    
    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator * (const Vector<T, dimension>& lhs, const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) * rhs);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<U, dimension> operator * (const T lhs, const Vector<U, dimension>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) * lhs);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator / (const Vector<T, dimension>& lhs, const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) / rhs);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<U, dimension> operator / (const T lhs, const Vector<U, dimension>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, lhs / v.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator + (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) + rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator - (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) - rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator * (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) * rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator / (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) / rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator += (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) + rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator -= (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) - rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator *= (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) * rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> operator /= (const Vector<T, dimension>& lhs, const Vector<U, dimension>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < dimension; i++){
            v.setVector(i, v.getVector(i) / rhs.getVector(i));
        }
        return v;
    }

    template<typename T, int dimension>
    inline Vector<float, dimension> Length(const Vector<T, dimension>& v)
    {
        float n = 0;
        for(int i = 0; i < dimension; i++){
            n = v.getVector(i) * v.getVector(i);
        }
        return std::sqrt(n);
    }

    template<typename T, int dimension>
    inline Vector<float, dimension> GetNormalized(const Vector<T, dimension>& v)
    {
        return v / Length(v);
    }

    template<typename T, int dimension>
    inline T Dot(const Vector<T, dimension>& lhs, const Vector<T, dimension>& rhs)
    {
        int val=0;
        for(int i = 0; i < dimension; i++){
            val += lhs.getVector(i) * rhs.getVector(i);
        }
        return val;
    }

    template<typename T>
    inline Vector<T, 3> Cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
    {
        return Vector<T, 3>({lhs.getVector(1) * rhs.getVector(2) - lhs.getVector(2) * rhs.getVector(1), lhs.getVector(2) * rhs.getVector(0) - lhs.getVector(0) * rhs.getVector(2), lhs.getVector(0) * rhs.getVector(1) - lhs.getVector(1) * rhs.getVector(0)});
    }

    template<typename T, int dimension>
    inline void Display_Vector(const Vector<T, dimension>& v)
    {
        std::cout << "{ ";
        
        for (int i = 0; i < dimension; i++){
                std::cout << v.getVector(i) << " ";
        }

        std::cout << "}" << std::endl;
    }

}