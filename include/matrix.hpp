#pragma once

#include <array>
#include <iostream>
#include <ostream>

namespace ShiftFlamework::math{

    template<typename T, int row , int column>
    class Matrix
    {
        public:
        Matrix(std::array<std::array <T, column>, row> init_value)
        {
            internal_data = init_value;
        }
        private:
        std::array<std::array <T, column>, row> internal_data;
        public:
        void setMatrix(int r, int c, T n);
        T getMatrix(int r, int c) const;

    };

    template<typename T, int row , int column>
    inline void Matrix<T, row, column>::setMatrix(int r, int c, T n){
        internal_data.at(r).at(c) = n;
    };

    template<typename T, int row , int column>
    inline T Matrix<T, row, column>::getMatrix(int r, int c) const
    {
        return internal_data.at(r).at(c);
    };

    template<typename T, int row , int column>
    inline Matrix<T, row , column> operator + (const Matrix<T, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j));
            }
        }
        return v;
    }

    template<typename T, int row , int column>
    inline Matrix<T, row , column> operator - (const Matrix<T, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, -v.getMatrix(i, j));
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<T, row , column> operator + (const Matrix<T, row , column>& lhs,const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j) + rhs);
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<U, row , column> operator + (const T lhs, const Matrix<U, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j) + lhs);
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<T, row , column> operator - (const Matrix<T, row , column>& lhs,const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j) - rhs);
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<U, row , column> operator - (const T lhs, const Matrix<U, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, lhs - v.getMatrix(i, j));
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<T, row , column> operator * (const Matrix<T, row , column>& lhs,const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j) * rhs);
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<U, row , column> operator * (const T lhs, const Matrix<U, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j) * lhs);
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<T, row , column> operator / (const Matrix<T, row , column>& lhs,const U rhs)
    {
        auto v = lhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, v.getMatrix(i, j) / rhs);
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<U, row , column> operator / (const T lhs, const Matrix<U, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, lhs / v.getMatrix(i, j));
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<U, row , column> operator + (const Matrix<T, row , column>& lhs, const Matrix<U, row , column>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, lhs.getMatrix(i, j) + rhs.getMatrix(i, j));
            }
        }
        return v;
    }

    template<typename T, int row , int column, typename U>
    inline Matrix<U, row , column> operator - (const Matrix<T, row , column>& lhs, const Matrix<U, row , column>& rhs)
    {
        auto v = lhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(i, j, lhs.getMatrix(i, j) - rhs.getMatrix(i, j));
            }
        }
        return v;
    }

    template<typename T, int row , int column, int num, typename U>
    inline Matrix<T, row , column> operator * (const Matrix<T, row , num>& lhs, const Matrix<U, num , column>& rhs)
    {
        auto v = Matrix<T, row , column> {{}};

        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                for(int k = 0; k < num; k++){
                    if(k==0){
                        v.setMatrix(i, j, 0);
                    }
                    v.setMatrix(i, j, v.getMatrix(i, j) + lhs.getMatrix(i, k) * rhs.getMatrix(k, j));
                }
            }
        }
        return v;
    }

    template<typename T, int size>
    inline Matrix<float, size, size> identity_matrixf(){
        
        auto v = Matrix<float, size , size> {{}};

        for(int i = 0; i < size; i++){
            for(int j=0; j < size; j++){
                if(i==j){
                    v.setMatrix(i, j, 1.0);
                }else{
                    v.setMatrix(i, j, 0.0);
                }
            }
        }
        return v;

    }

    template<typename T, int size>
    inline Matrix<double, size, size> identity_matrixd(){
        
        auto v = Matrix<double, size , size> {{}};

        for(int i = 0; i < size; i++){
            for(int j=0; j < size; j++){
                if(i==j){
                    v.setMatrix(i, j, 1.0);
                }else{
                    v.setMatrix(i, j, 0.0);
                }
            }
        }
        return v;

    }

    template<typename T, int size>
    inline Matrix<int, size, size> identity_matrixi(){
        
        auto v = Matrix<int, size , size> {{}};

        for(int i = 0; i < size; i++){
            for(int j=0; j < size; j++){
                if(i==j){
                    v.setMatrix(i, j, 1);
                }else{
                    v.setMatrix(i, j, 0);
                }
            }
        }
        return v;

    }

    template<typename T>
    inline Matrix<float, 2, 2> inverse_matrix2(const Matrix<T, 2, 2>& mat){
        auto m = Matrix<float, 2, 2> {{}};
        float dat = mat.getMatrix(0, 0) * mat.getMatrix(1, 1) - mat.getMatrix(0, 1) * mat.getMatrix(1, 0);
        m.setMatrix(0, 0, mat.getMatrix(1, 1) / dat);
        m.setMatrix(0, 1, -mat.getMatrix(0, 1) / dat);
        m.setMatrix(1, 0, -mat.getMatrix(1, 0) / dat);
        m.setMatrix(1, 1, mat.getMatrix(0, 0) / dat);

        return m;
    }

    template<typename T>
    inline Matrix<float, 3, 3> inverse_matrix3(const Matrix<T, 3, 3>& mat){
        auto m = Matrix<float, 3, 3> {{}};
        float dat = mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(2, 2)
        + mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(2, 0)
        + mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(2, 1)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(2, 0)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(2, 2)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(2, 1);

        m.setMatrix(0, 0, (mat.getMatrix(1, 1) * mat.getMatrix(2, 2) - mat.getMatrix(1, 2) * mat.getMatrix(2, 1)) / dat);
        m.setMatrix(0, 1, (mat.getMatrix(0, 2) * mat.getMatrix(2, 1) - mat.getMatrix(0, 1) * mat.getMatrix(2, 2)) / dat);
        m.setMatrix(0, 2, (mat.getMatrix(0, 1) * mat.getMatrix(1, 2) - mat.getMatrix(0, 2) * mat.getMatrix(1, 1)) / dat);
        m.setMatrix(1, 0, (mat.getMatrix(1, 2) * mat.getMatrix(2, 0) - mat.getMatrix(1, 0) * mat.getMatrix(2, 2)) / dat);
        m.setMatrix(1, 1, (mat.getMatrix(0, 0) * mat.getMatrix(2, 2) - mat.getMatrix(0, 2) * mat.getMatrix(2, 0)) / dat);
        m.setMatrix(1, 2, (mat.getMatrix(0, 2) * mat.getMatrix(1, 0) - mat.getMatrix(0, 0) * mat.getMatrix(1, 2)) / dat);
        m.setMatrix(2, 0, (mat.getMatrix(1, 0) * mat.getMatrix(2, 1) - mat.getMatrix(1, 1) * mat.getMatrix(2, 0)) / dat);
        m.setMatrix(2, 1, (mat.getMatrix(0, 1) * mat.getMatrix(2, 0) - mat.getMatrix(0, 0) * mat.getMatrix(2, 1)) / dat);
        m.setMatrix(2, 2, (mat.getMatrix(0, 0) * mat.getMatrix(1, 1) - mat.getMatrix(0, 1) * mat.getMatrix(1, 0)) / dat);

        return m;
    }

    template<typename T>
    inline Matrix<float, 4, 4> inverse_matrix4(const Matrix<T, 4 , 4>& mat){
        auto m = Matrix<float, 4 , 4> {{}};
        float dat = mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(2, 2) * mat.getMatrix(3, 3)
        +mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(2, 3) * mat.getMatrix(3, 1)
        +mat.getMatrix(0, 0) * mat.getMatrix(1, 3) * mat.getMatrix(2, 1) * mat.getMatrix(3, 2)
        -mat.getMatrix(0, 0) * mat.getMatrix(1, 3) * mat.getMatrix(2, 2) * mat.getMatrix(3, 1)
        -mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(2, 1) * mat.getMatrix(3, 3)
        -mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(2, 3) * mat.getMatrix(3, 2)
        -mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(2, 2) * mat.getMatrix(3, 3)
        -mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(2, 3) * mat.getMatrix(3, 1)
        -mat.getMatrix(0, 3) * mat.getMatrix(1, 0) * mat.getMatrix(2, 1) * mat.getMatrix(3, 2)
        +mat.getMatrix(0, 3) * mat.getMatrix(1, 0) * mat.getMatrix(2, 2) * mat.getMatrix(3, 1)
        +mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(2, 1) * mat.getMatrix(3, 3)
        +mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(2, 3) * mat.getMatrix(3, 2)
        +mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(2, 0) * mat.getMatrix(3, 3)
        +mat.getMatrix(0, 2) * mat.getMatrix(1, 3) * mat.getMatrix(2, 0) * mat.getMatrix(3, 1)
        +mat.getMatrix(0, 3) * mat.getMatrix(1, 1) * mat.getMatrix(2, 0) * mat.getMatrix(3, 2)
        -mat.getMatrix(0, 3) * mat.getMatrix(1, 2) * mat.getMatrix(2, 0) * mat.getMatrix(3, 1)
        -mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(2, 0) * mat.getMatrix(3, 3)
        -mat.getMatrix(0, 1) * mat.getMatrix(1, 3) * mat.getMatrix(2, 0) * mat.getMatrix(3, 2)
        -mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(2, 3) * mat.getMatrix(3, 0)
        -mat.getMatrix(0, 2) * mat.getMatrix(1, 3) * mat.getMatrix(2, 1) * mat.getMatrix(3, 0)
        -mat.getMatrix(0, 3) * mat.getMatrix(1, 1) * mat.getMatrix(2, 2) * mat.getMatrix(3, 0)
        +mat.getMatrix(0, 3) * mat.getMatrix(1, 2) * mat.getMatrix(2, 1) * mat.getMatrix(3, 0)
        +mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(2, 3) * mat.getMatrix(3, 0)
        +mat.getMatrix(0, 1) * mat.getMatrix(1, 3) * mat.getMatrix(2, 2) * mat.getMatrix(3, 0);

        m.setMatrix(0, 0, (mat.getMatrix(1, 1) * mat.getMatrix(2, 2) * mat.getMatrix(3, 3) 
        + mat.getMatrix(1, 2) * mat.getMatrix(2, 3) * mat.getMatrix(3, 1)
        + mat.getMatrix(1, 3) * mat.getMatrix(2, 1) * mat.getMatrix(3, 2)
        - mat.getMatrix(1, 3) * mat.getMatrix(2, 2) * mat.getMatrix(3, 1)
        - mat.getMatrix(1, 2) * mat.getMatrix(2, 1) * mat.getMatrix(3, 3)
        - mat.getMatrix(1, 1) * mat.getMatrix(2, 3) * mat.getMatrix(3, 2)) / dat);
        m.setMatrix(0, 1, (mat.getMatrix(0, 3) * mat.getMatrix(2, 2) * mat.getMatrix(3, 1) 
        + mat.getMatrix(0, 2) * mat.getMatrix(2, 1) * mat.getMatrix(3, 3)
        + mat.getMatrix(0, 1) * mat.getMatrix(2, 3) * mat.getMatrix(3, 2)
        - mat.getMatrix(0, 1) * mat.getMatrix(2, 2) * mat.getMatrix(3, 3)
        - mat.getMatrix(0, 2) * mat.getMatrix(2, 3) * mat.getMatrix(3, 1)
        - mat.getMatrix(0, 3) * mat.getMatrix(2, 1) * mat.getMatrix(3, 2)) / dat);
        m.setMatrix(0, 2, (mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(3, 3) 
        + mat.getMatrix(0, 2) * mat.getMatrix(1, 3) * mat.getMatrix(3, 1)
        + mat.getMatrix(0, 3) * mat.getMatrix(1, 1) * mat.getMatrix(3, 2)
        - mat.getMatrix(0, 3) * mat.getMatrix(1, 2) * mat.getMatrix(3, 1)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(3, 3)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 3) * mat.getMatrix(3, 2)) / dat);
        m.setMatrix(0, 3, (mat.getMatrix(0, 3) * mat.getMatrix(1, 2) * mat.getMatrix(2, 1) 
        + mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(2, 3)
        + mat.getMatrix(0, 1) * mat.getMatrix(1, 3) * mat.getMatrix(2, 2)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(2, 3)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 3) * mat.getMatrix(2, 1)
        - mat.getMatrix(0, 3) * mat.getMatrix(1, 1) * mat.getMatrix(2, 2)) / dat);
        m.setMatrix(1, 0, (mat.getMatrix(1, 3) * mat.getMatrix(2, 2) * mat.getMatrix(3, 0) 
        + mat.getMatrix(1, 2) * mat.getMatrix(2, 0) * mat.getMatrix(3, 3)
        + mat.getMatrix(1, 0) * mat.getMatrix(2, 3) * mat.getMatrix(3, 2)
        - mat.getMatrix(1, 0) * mat.getMatrix(2, 2) * mat.getMatrix(3, 3)
        - mat.getMatrix(1, 2) * mat.getMatrix(2, 3) * mat.getMatrix(3, 0)
        - mat.getMatrix(1, 3) * mat.getMatrix(2, 0) * mat.getMatrix(3, 2)) / dat);
        m.setMatrix(1, 1, (mat.getMatrix(0, 0) * mat.getMatrix(2, 2) * mat.getMatrix(3, 3) 
        + mat.getMatrix(0, 2) * mat.getMatrix(2, 3) * mat.getMatrix(3, 0)
        + mat.getMatrix(0, 3) * mat.getMatrix(2, 0) * mat.getMatrix(3, 2)
        - mat.getMatrix(0, 3) * mat.getMatrix(2, 2) * mat.getMatrix(3, 0)
        - mat.getMatrix(0, 2) * mat.getMatrix(2, 0) * mat.getMatrix(3, 3)
        - mat.getMatrix(0, 0) * mat.getMatrix(2, 3) * mat.getMatrix(3, 2)) / dat);
        m.setMatrix(1, 2, (mat.getMatrix(0, 3) * mat.getMatrix(1, 2) * mat.getMatrix(3, 0) 
        + mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(3, 3)
        + mat.getMatrix(0, 0) * mat.getMatrix(1, 3) * mat.getMatrix(3, 2)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(3, 3)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 3) * mat.getMatrix(3, 0)
        - mat.getMatrix(0, 3) * mat.getMatrix(1, 0) * mat.getMatrix(3, 2)) / dat);
        m.setMatrix(1, 3, (mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(2, 3) 
        + mat.getMatrix(0, 2) * mat.getMatrix(1, 3) * mat.getMatrix(2, 0)
        + mat.getMatrix(0, 3) * mat.getMatrix(1, 0) * mat.getMatrix(2, 2)
        - mat.getMatrix(0, 3) * mat.getMatrix(1, 2) * mat.getMatrix(2, 0)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(2, 3)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 3) * mat.getMatrix(2, 2)) / dat);
        m.setMatrix(2, 0, (mat.getMatrix(1, 0) * mat.getMatrix(2, 1) * mat.getMatrix(3, 3) 
        + mat.getMatrix(1, 1) * mat.getMatrix(2, 3) * mat.getMatrix(3, 0)
        + mat.getMatrix(1, 3) * mat.getMatrix(2, 0) * mat.getMatrix(3, 1)
        - mat.getMatrix(1, 3) * mat.getMatrix(2, 1) * mat.getMatrix(3, 0)
        - mat.getMatrix(1, 1) * mat.getMatrix(2, 0) * mat.getMatrix(3, 3)
        - mat.getMatrix(1, 0) * mat.getMatrix(2, 3) * mat.getMatrix(3, 1)) / dat);
        m.setMatrix(2, 1, (mat.getMatrix(0, 3) * mat.getMatrix(2, 1) * mat.getMatrix(3, 0) 
        + mat.getMatrix(0, 1) * mat.getMatrix(2, 0) * mat.getMatrix(3, 3)
        + mat.getMatrix(0, 0) * mat.getMatrix(2, 3) * mat.getMatrix(3, 1)
        - mat.getMatrix(0, 0) * mat.getMatrix(2, 1) * mat.getMatrix(3, 3)
        - mat.getMatrix(0, 1) * mat.getMatrix(2, 3) * mat.getMatrix(3, 0)
        - mat.getMatrix(0, 3) * mat.getMatrix(2, 0) * mat.getMatrix(3, 1)) / dat);
        m.setMatrix(2, 2, (mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(3, 3) 
        + mat.getMatrix(0, 1) * mat.getMatrix(1, 3) * mat.getMatrix(3, 0)
        + mat.getMatrix(0, 3) * mat.getMatrix(1, 0) * mat.getMatrix(3, 1)
        - mat.getMatrix(0, 3) * mat.getMatrix(1, 1) * mat.getMatrix(3, 0)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(3, 3)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 3) * mat.getMatrix(3, 1)) / dat);
        m.setMatrix(2, 3, (mat.getMatrix(0, 3) * mat.getMatrix(1, 1) * mat.getMatrix(2, 0) 
        + mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(2, 3)
        + mat.getMatrix(0, 0) * mat.getMatrix(1, 3) * mat.getMatrix(2, 1)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(2, 3)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 3) * mat.getMatrix(2, 0)
        - mat.getMatrix(0, 3) * mat.getMatrix(1, 0) * mat.getMatrix(2, 1)) / dat);
        m.setMatrix(3, 0, (mat.getMatrix(1, 2) * mat.getMatrix(2, 1) * mat.getMatrix(3, 0) 
        + mat.getMatrix(1, 1) * mat.getMatrix(2, 0) * mat.getMatrix(3, 2)
        + mat.getMatrix(1, 0) * mat.getMatrix(2, 2) * mat.getMatrix(3, 1)
        - mat.getMatrix(1, 0) * mat.getMatrix(2, 1) * mat.getMatrix(3, 2)
        - mat.getMatrix(1, 1) * mat.getMatrix(2, 2) * mat.getMatrix(3, 0)
        - mat.getMatrix(1, 2) * mat.getMatrix(2, 0) * mat.getMatrix(3, 1)) / dat);
        m.setMatrix(3, 1, (mat.getMatrix(0, 0) * mat.getMatrix(2, 1) * mat.getMatrix(3, 2) 
        + mat.getMatrix(0, 1) * mat.getMatrix(2, 2) * mat.getMatrix(3, 0)
        + mat.getMatrix(0, 2) * mat.getMatrix(2, 0) * mat.getMatrix(3, 1)
        - mat.getMatrix(0, 2) * mat.getMatrix(2, 1) * mat.getMatrix(3, 0)
        - mat.getMatrix(0, 1) * mat.getMatrix(2, 0) * mat.getMatrix(3, 2)
        - mat.getMatrix(0, 0) * mat.getMatrix(2, 2) * mat.getMatrix(3, 1)) / dat);
        m.setMatrix(3, 2, (mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(3, 0) 
        + mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(3, 2)
        + mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(3, 1)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(3, 2)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(3, 0)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(3, 1)) / dat);
        m.setMatrix(3, 3, (mat.getMatrix(0, 0) * mat.getMatrix(1, 1) * mat.getMatrix(2, 2) 
        + mat.getMatrix(0, 1) * mat.getMatrix(1, 2) * mat.getMatrix(2, 0)
        + mat.getMatrix(0, 2) * mat.getMatrix(1, 0) * mat.getMatrix(2, 1)
        - mat.getMatrix(0, 2) * mat.getMatrix(1, 1) * mat.getMatrix(2, 0)
        - mat.getMatrix(0, 1) * mat.getMatrix(1, 0) * mat.getMatrix(2, 2)
        - mat.getMatrix(0, 0) * mat.getMatrix(1, 2) * mat.getMatrix(2, 1)) / dat);

        return m;
    }

    template<typename T, int row , int column>
    inline Matrix<T, column , row> transpose_matrix(const Matrix<T , row, column>& m)
    {
        auto v = Matrix<T, column , row> {{}};
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setMatrix(j, i, m.getMatrix(i, j));
            }
        }

        return v;
    }



    template<typename T, int row , int column>
    inline void Display_Matrix(const Matrix<T , row, column>& m)
    {
        std::cout << "{ ";
        
        for (int i = 0; i < row; i++){
            if(i!=0){
                std::cout << "  ";
            }
            for (int j = 0; j < column; j++){
                std::cout << m.getMatrix(i, j) << " ";
            }
            if(i!=row-1){
                std::cout << std::endl;
            }
        }

        std::cout << "}" << std::endl;
    }

}