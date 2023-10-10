#pragma once

#include <array>
#include <iostream>
#include <ostream>

namespace ShiftFlamework::Math{

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
        void set_matrix(int r, int c, T n);
        T get_matrix(int r, int c) const;

    };

    template<typename T, int row , int column>
    inline void Matrix<T, row, column>::set_matrix(int r, int c, T n){
        internal_data.at(r).at(c) = n;
    };

    template<typename T, int row , int column>
    inline T Matrix<T, row, column>::get_matrix(int r, int c) const
    {
        return internal_data.at(r).at(c);
    };

    template<typename T, int row , int column>
    inline Matrix<T, row , column> operator + (const Matrix<T, row , column>& rhs)
    {
        auto v = rhs;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setmatrix(i, j, v.getmatrix(i, j));
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
                v.setmatrix(i, j, -v.getmatrix(i, j));
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
                v.setmatrix(i, j, v.getmatrix(i, j) + rhs);
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
                v.setmatrix(i, j, v.getmatrix(i, j) + lhs);
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
                v.setmatrix(i, j, v.getmatrix(i, j) - rhs);
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
                v.setmatrix(i, j, lhs - v.getmatrix(i, j));
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
                v.setmatrix(i, j, v.getmatrix(i, j) * rhs);
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
                v.setmatrix(i, j, v.getmatrix(i, j) * lhs);
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
                v.setmatrix(i, j, v.getmatrix(i, j) / rhs);
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
                v.setmatrix(i, j, lhs / v.getmatrix(i, j));
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
                v.setmatrix(i, j, lhs.get_matrix(i, j) + rhs.get_matrix(i, j));
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
                v.setmatrix(i, j, lhs.get_matrix(i, j) - rhs.get_matrix(i, j));
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
                        v.setmatrix(i, j, 0);
                    }
                    v.setmatrix(i, j, v.getmatrix(i, j) + lhs.get_matrix(i, k) * rhs.get_matrix(k, j));
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
                    v.setmatrix(i, j, 1.0);
                }else{
                    v.setmatrix(i, j, 0.0);
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
                    v.setmatrix(i, j, 1.0);
                }else{
                    v.setmatrix(i, j, 0.0);
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
                    v.setmatrix(i, j, 1);
                }else{
                    v.setmatrix(i, j, 0);
                }
            }
        }
        return v;

    }

    template<typename T>
    inline Matrix<float, 2, 2> inverse_matrix2(const Matrix<T, 2, 2>& mat){
        auto m = Matrix<float, 2, 2> {{}};
        float dat = mat.get_matrix(0, 0) * mat.get_matrix(1, 1) - mat.get_matrix(0, 1) * mat.get_matrix(1, 0);
        m.set_matrix(0, 0, mat.get_matrix(1, 1) / dat);
        m.set_matrix(0, 1, -mat.get_matrix(0, 1) / dat);
        m.set_matrix(1, 0, -mat.get_matrix(1, 0) / dat);
        m.set_matrix(1, 1, mat.get_matrix(0, 0) / dat);

        return m;
    }

    template<typename T>
    inline Matrix<float, 3, 3> inverse_matrix3(const Matrix<T, 3, 3>& mat){
        auto m = Matrix<float, 3, 3> {{}};
        float dat = mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(2, 2)
        + mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(2, 0)
        + mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(2, 1)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(2, 0)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(2, 2)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(2, 1);

        m.set_matrix(0, 0, (mat.get_matrix(1, 1) * mat.get_matrix(2, 2) - mat.get_matrix(1, 2) * mat.get_matrix(2, 1)) / dat);
        m.set_matrix(0, 1, (mat.get_matrix(0, 2) * mat.get_matrix(2, 1) - mat.get_matrix(0, 1) * mat.get_matrix(2, 2)) / dat);
        m.set_matrix(0, 2, (mat.get_matrix(0, 1) * mat.get_matrix(1, 2) - mat.get_matrix(0, 2) * mat.get_matrix(1, 1)) / dat);
        m.set_matrix(1, 0, (mat.get_matrix(1, 2) * mat.get_matrix(2, 0) - mat.get_matrix(1, 0) * mat.get_matrix(2, 2)) / dat);
        m.set_matrix(1, 1, (mat.get_matrix(0, 0) * mat.get_matrix(2, 2) - mat.get_matrix(0, 2) * mat.get_matrix(2, 0)) / dat);
        m.set_matrix(1, 2, (mat.get_matrix(0, 2) * mat.get_matrix(1, 0) - mat.get_matrix(0, 0) * mat.get_matrix(1, 2)) / dat);
        m.set_matrix(2, 0, (mat.get_matrix(1, 0) * mat.get_matrix(2, 1) - mat.get_matrix(1, 1) * mat.get_matrix(2, 0)) / dat);
        m.set_matrix(2, 1, (mat.get_matrix(0, 1) * mat.get_matrix(2, 0) - mat.get_matrix(0, 0) * mat.get_matrix(2, 1)) / dat);
        m.set_matrix(2, 2, (mat.get_matrix(0, 0) * mat.get_matrix(1, 1) - mat.get_matrix(0, 1) * mat.get_matrix(1, 0)) / dat);

        return m;
    }

    template<typename T>
    inline Matrix<float, 4, 4> inverse_matrix4(const Matrix<T, 4 , 4>& mat){
        auto m = Matrix<float, 4 , 4> {{}};
        float dat = mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(2, 2) * mat.get_matrix(3, 3)
        +mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(2, 3) * mat.get_matrix(3, 1)
        +mat.get_matrix(0, 0) * mat.get_matrix(1, 3) * mat.get_matrix(2, 1) * mat.get_matrix(3, 2)
        -mat.get_matrix(0, 0) * mat.get_matrix(1, 3) * mat.get_matrix(2, 2) * mat.get_matrix(3, 1)
        -mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(2, 1) * mat.get_matrix(3, 3)
        -mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(2, 3) * mat.get_matrix(3, 2)
        -mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(2, 2) * mat.get_matrix(3, 3)
        -mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(2, 3) * mat.get_matrix(3, 1)
        -mat.get_matrix(0, 3) * mat.get_matrix(1, 0) * mat.get_matrix(2, 1) * mat.get_matrix(3, 2)
        +mat.get_matrix(0, 3) * mat.get_matrix(1, 0) * mat.get_matrix(2, 2) * mat.get_matrix(3, 1)
        +mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(2, 1) * mat.get_matrix(3, 3)
        +mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(2, 3) * mat.get_matrix(3, 2)
        +mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(2, 0) * mat.get_matrix(3, 3)
        +mat.get_matrix(0, 2) * mat.get_matrix(1, 3) * mat.get_matrix(2, 0) * mat.get_matrix(3, 1)
        +mat.get_matrix(0, 3) * mat.get_matrix(1, 1) * mat.get_matrix(2, 0) * mat.get_matrix(3, 2)
        -mat.get_matrix(0, 3) * mat.get_matrix(1, 2) * mat.get_matrix(2, 0) * mat.get_matrix(3, 1)
        -mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(2, 0) * mat.get_matrix(3, 3)
        -mat.get_matrix(0, 1) * mat.get_matrix(1, 3) * mat.get_matrix(2, 0) * mat.get_matrix(3, 2)
        -mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(2, 3) * mat.get_matrix(3, 0)
        -mat.get_matrix(0, 2) * mat.get_matrix(1, 3) * mat.get_matrix(2, 1) * mat.get_matrix(3, 0)
        -mat.get_matrix(0, 3) * mat.get_matrix(1, 1) * mat.get_matrix(2, 2) * mat.get_matrix(3, 0)
        +mat.get_matrix(0, 3) * mat.get_matrix(1, 2) * mat.get_matrix(2, 1) * mat.get_matrix(3, 0)
        +mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(2, 3) * mat.get_matrix(3, 0)
        +mat.get_matrix(0, 1) * mat.get_matrix(1, 3) * mat.get_matrix(2, 2) * mat.get_matrix(3, 0);

        m.set_matrix(0, 0, (mat.get_matrix(1, 1) * mat.get_matrix(2, 2) * mat.get_matrix(3, 3) 
        + mat.get_matrix(1, 2) * mat.get_matrix(2, 3) * mat.get_matrix(3, 1)
        + mat.get_matrix(1, 3) * mat.get_matrix(2, 1) * mat.get_matrix(3, 2)
        - mat.get_matrix(1, 3) * mat.get_matrix(2, 2) * mat.get_matrix(3, 1)
        - mat.get_matrix(1, 2) * mat.get_matrix(2, 1) * mat.get_matrix(3, 3)
        - mat.get_matrix(1, 1) * mat.get_matrix(2, 3) * mat.get_matrix(3, 2)) / dat);
        m.set_matrix(0, 1, (mat.get_matrix(0, 3) * mat.get_matrix(2, 2) * mat.get_matrix(3, 1) 
        + mat.get_matrix(0, 2) * mat.get_matrix(2, 1) * mat.get_matrix(3, 3)
        + mat.get_matrix(0, 1) * mat.get_matrix(2, 3) * mat.get_matrix(3, 2)
        - mat.get_matrix(0, 1) * mat.get_matrix(2, 2) * mat.get_matrix(3, 3)
        - mat.get_matrix(0, 2) * mat.get_matrix(2, 3) * mat.get_matrix(3, 1)
        - mat.get_matrix(0, 3) * mat.get_matrix(2, 1) * mat.get_matrix(3, 2)) / dat);
        m.set_matrix(0, 2, (mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(3, 3) 
        + mat.get_matrix(0, 2) * mat.get_matrix(1, 3) * mat.get_matrix(3, 1)
        + mat.get_matrix(0, 3) * mat.get_matrix(1, 1) * mat.get_matrix(3, 2)
        - mat.get_matrix(0, 3) * mat.get_matrix(1, 2) * mat.get_matrix(3, 1)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(3, 3)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 3) * mat.get_matrix(3, 2)) / dat);
        m.set_matrix(0, 3, (mat.get_matrix(0, 3) * mat.get_matrix(1, 2) * mat.get_matrix(2, 1) 
        + mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(2, 3)
        + mat.get_matrix(0, 1) * mat.get_matrix(1, 3) * mat.get_matrix(2, 2)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(2, 3)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 3) * mat.get_matrix(2, 1)
        - mat.get_matrix(0, 3) * mat.get_matrix(1, 1) * mat.get_matrix(2, 2)) / dat);
        m.set_matrix(1, 0, (mat.get_matrix(1, 3) * mat.get_matrix(2, 2) * mat.get_matrix(3, 0) 
        + mat.get_matrix(1, 2) * mat.get_matrix(2, 0) * mat.get_matrix(3, 3)
        + mat.get_matrix(1, 0) * mat.get_matrix(2, 3) * mat.get_matrix(3, 2)
        - mat.get_matrix(1, 0) * mat.get_matrix(2, 2) * mat.get_matrix(3, 3)
        - mat.get_matrix(1, 2) * mat.get_matrix(2, 3) * mat.get_matrix(3, 0)
        - mat.get_matrix(1, 3) * mat.get_matrix(2, 0) * mat.get_matrix(3, 2)) / dat);
        m.set_matrix(1, 1, (mat.get_matrix(0, 0) * mat.get_matrix(2, 2) * mat.get_matrix(3, 3) 
        + mat.get_matrix(0, 2) * mat.get_matrix(2, 3) * mat.get_matrix(3, 0)
        + mat.get_matrix(0, 3) * mat.get_matrix(2, 0) * mat.get_matrix(3, 2)
        - mat.get_matrix(0, 3) * mat.get_matrix(2, 2) * mat.get_matrix(3, 0)
        - mat.get_matrix(0, 2) * mat.get_matrix(2, 0) * mat.get_matrix(3, 3)
        - mat.get_matrix(0, 0) * mat.get_matrix(2, 3) * mat.get_matrix(3, 2)) / dat);
        m.set_matrix(1, 2, (mat.get_matrix(0, 3) * mat.get_matrix(1, 2) * mat.get_matrix(3, 0) 
        + mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(3, 3)
        + mat.get_matrix(0, 0) * mat.get_matrix(1, 3) * mat.get_matrix(3, 2)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(3, 3)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 3) * mat.get_matrix(3, 0)
        - mat.get_matrix(0, 3) * mat.get_matrix(1, 0) * mat.get_matrix(3, 2)) / dat);
        m.set_matrix(1, 3, (mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(2, 3) 
        + mat.get_matrix(0, 2) * mat.get_matrix(1, 3) * mat.get_matrix(2, 0)
        + mat.get_matrix(0, 3) * mat.get_matrix(1, 0) * mat.get_matrix(2, 2)
        - mat.get_matrix(0, 3) * mat.get_matrix(1, 2) * mat.get_matrix(2, 0)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(2, 3)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 3) * mat.get_matrix(2, 2)) / dat);
        m.set_matrix(2, 0, (mat.get_matrix(1, 0) * mat.get_matrix(2, 1) * mat.get_matrix(3, 3) 
        + mat.get_matrix(1, 1) * mat.get_matrix(2, 3) * mat.get_matrix(3, 0)
        + mat.get_matrix(1, 3) * mat.get_matrix(2, 0) * mat.get_matrix(3, 1)
        - mat.get_matrix(1, 3) * mat.get_matrix(2, 1) * mat.get_matrix(3, 0)
        - mat.get_matrix(1, 1) * mat.get_matrix(2, 0) * mat.get_matrix(3, 3)
        - mat.get_matrix(1, 0) * mat.get_matrix(2, 3) * mat.get_matrix(3, 1)) / dat);
        m.set_matrix(2, 1, (mat.get_matrix(0, 3) * mat.get_matrix(2, 1) * mat.get_matrix(3, 0) 
        + mat.get_matrix(0, 1) * mat.get_matrix(2, 0) * mat.get_matrix(3, 3)
        + mat.get_matrix(0, 0) * mat.get_matrix(2, 3) * mat.get_matrix(3, 1)
        - mat.get_matrix(0, 0) * mat.get_matrix(2, 1) * mat.get_matrix(3, 3)
        - mat.get_matrix(0, 1) * mat.get_matrix(2, 3) * mat.get_matrix(3, 0)
        - mat.get_matrix(0, 3) * mat.get_matrix(2, 0) * mat.get_matrix(3, 1)) / dat);
        m.set_matrix(2, 2, (mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(3, 3) 
        + mat.get_matrix(0, 1) * mat.get_matrix(1, 3) * mat.get_matrix(3, 0)
        + mat.get_matrix(0, 3) * mat.get_matrix(1, 0) * mat.get_matrix(3, 1)
        - mat.get_matrix(0, 3) * mat.get_matrix(1, 1) * mat.get_matrix(3, 0)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(3, 3)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 3) * mat.get_matrix(3, 1)) / dat);
        m.set_matrix(2, 3, (mat.get_matrix(0, 3) * mat.get_matrix(1, 1) * mat.get_matrix(2, 0) 
        + mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(2, 3)
        + mat.get_matrix(0, 0) * mat.get_matrix(1, 3) * mat.get_matrix(2, 1)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(2, 3)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 3) * mat.get_matrix(2, 0)
        - mat.get_matrix(0, 3) * mat.get_matrix(1, 0) * mat.get_matrix(2, 1)) / dat);
        m.set_matrix(3, 0, (mat.get_matrix(1, 2) * mat.get_matrix(2, 1) * mat.get_matrix(3, 0) 
        + mat.get_matrix(1, 1) * mat.get_matrix(2, 0) * mat.get_matrix(3, 2)
        + mat.get_matrix(1, 0) * mat.get_matrix(2, 2) * mat.get_matrix(3, 1)
        - mat.get_matrix(1, 0) * mat.get_matrix(2, 1) * mat.get_matrix(3, 2)
        - mat.get_matrix(1, 1) * mat.get_matrix(2, 2) * mat.get_matrix(3, 0)
        - mat.get_matrix(1, 2) * mat.get_matrix(2, 0) * mat.get_matrix(3, 1)) / dat);
        m.set_matrix(3, 1, (mat.get_matrix(0, 0) * mat.get_matrix(2, 1) * mat.get_matrix(3, 2) 
        + mat.get_matrix(0, 1) * mat.get_matrix(2, 2) * mat.get_matrix(3, 0)
        + mat.get_matrix(0, 2) * mat.get_matrix(2, 0) * mat.get_matrix(3, 1)
        - mat.get_matrix(0, 2) * mat.get_matrix(2, 1) * mat.get_matrix(3, 0)
        - mat.get_matrix(0, 1) * mat.get_matrix(2, 0) * mat.get_matrix(3, 2)
        - mat.get_matrix(0, 0) * mat.get_matrix(2, 2) * mat.get_matrix(3, 1)) / dat);
        m.set_matrix(3, 2, (mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(3, 0) 
        + mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(3, 2)
        + mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(3, 1)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(3, 2)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(3, 0)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(3, 1)) / dat);
        m.set_matrix(3, 3, (mat.get_matrix(0, 0) * mat.get_matrix(1, 1) * mat.get_matrix(2, 2) 
        + mat.get_matrix(0, 1) * mat.get_matrix(1, 2) * mat.get_matrix(2, 0)
        + mat.get_matrix(0, 2) * mat.get_matrix(1, 0) * mat.get_matrix(2, 1)
        - mat.get_matrix(0, 2) * mat.get_matrix(1, 1) * mat.get_matrix(2, 0)
        - mat.get_matrix(0, 1) * mat.get_matrix(1, 0) * mat.get_matrix(2, 2)
        - mat.get_matrix(0, 0) * mat.get_matrix(1, 2) * mat.get_matrix(2, 1)) / dat);

        return m;
    }

    template<typename T, int row , int column>
    inline Matrix<T, column , row> transpose_matrix(const Matrix<T , row, column>& m)
    {
        auto v = Matrix<T, column , row> {{}};
        for(int i = 0; i < row; i++){
            for(int j = 0; j < column; j++){
                v.setmatrix(j, i, m.get_matrix(i, j));
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
                std::cout << m.get_matrix(i, j) << " ";
            }
            if(i!=row-1){
                std::cout << std::endl;
            }
        }

        std::cout << "}" << std::endl;
    }

}