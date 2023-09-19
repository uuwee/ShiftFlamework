#pragma once

#include <array>
#include <iostream>
#include <ostream>

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
    inline Vector<float, dimension> length(const Vector<T, dimension>& v)
    {
        float n = 0;
        for(int i = 0; i < dimension; i++){
            n = v.getVector(i) * v.getVector(i);
        }
        return std::sqrt(n);
    }

    template<typename T, int dimension>
    inline Vector<float, dimension> normalized(const Vector<T, dimension>& v)
    {
        return v / length(v);
    }

    template<typename T, int dimension>
    inline T dot(const Vector<T, dimension>& lhs, const Vector<T, dimension>& rhs)
    {
        int val=0;
        for(int i = 0; i < dimension; i++){
            val += lhs.getVector(i) * rhs.getVector(i);
        }
        return val;
    }

    template<typename T>
    inline Vector<T, 3> cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
    {
        return Vector<T, 3>({lhs.getVector(1) * rhs.getVector(2) - lhs.getVector(2) * rhs.getVector(1), lhs.getVector(2) * rhs.getVector(0) - lhs.getVector(0) * rhs.getVector(2), lhs.getVector(0) * rhs.getVector(1) - lhs.getVector(1) * rhs.getVector(0)});
    }

    template<typename T, int dimension>
    inline Vector<T, dimension> one()
    {
        auto v = Vector<T, dimension>{{}};
        for(int i = 0; i < dimension; i++){
            v.setVector(i, 1);
        }
        return v;
    }

    template<typename T, int dimension>
    inline Vector<T, dimension> zero()
    {
        auto v = Vector<T, dimension>{{}};
        for(int i = 0; i < dimension; i++){
            v.setVector(i, 0);
        }
        return v;
    }

    template<typename T>
    inline Vector<T, 3> forward()
    {
        return Vector<T, 3>({0,1,0});
    }

    template<typename T>
    inline Vector<T, 3> back()
    {
        return Vector<T, 3>({0,-1,0});
    }

    template<typename T>
    inline Vector<T, 3> right()
    {
        return Vector<T, 3>({1,0,0});
    }

    template<typename T>
    inline Vector<T, 3> left()
    {
        return Vector<T, 3>({-1,0,0});
    }

    template<typename T>
    inline Vector<T, 3> up()
    {
        return Vector<T, 3>({0,0,1});
    }

    template<typename T>
    inline Vector<T, 3> down()
    {
        return Vector<T, 3>({0,0,-1});
    }

    template<typename T, int dimension, typename U>
    inline Vector<T, dimension> distance(const Vector<T, dimension>& v1, const Vector<U, dimension>& v2)
    {
        return length(v1 - v2);
    }

    template<typename T, int dimension, typename U>
    inline Vector<float, dimension> movetoword(const Vector<T, dimension>& from, const Vector<U, dimension>& to, float per)
    {
        auto v = Vector<float, dimension>{{}};
        for(int i = 0; i < dimension; i++){
            v.setVector(i, from.getVector(i) + to.getVector(i)*per);
        }
        return v;
    }

    template<typename T, int dimension, typename U>
    inline Vector<float, dimension> lerp(const Vector<T, dimension>& from, const Vector<U, dimension>& to, float per)
    {
        auto v = Vector<float, dimension>{{}};
        for(int i = 0; i < dimension; i++){
            v.setVector(i, from.getVector(i)*(1-per) + to.getVector(i)*per);
        }
        return v;
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