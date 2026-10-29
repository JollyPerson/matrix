#pragma once

#include "matrix.hpp"

template <MatrixElement T>
Matrix<T> operator+(Matrix<T> lhs, const Matrix<T>& rhs);

template <MatrixElement T>
Matrix<T> operator-(Matrix<T> lhs, const Matrix<T>& rhs);

template <MatrixElement T>
Matrix<T> operator*(Matrix<T> matrix, T scalar);

template <MatrixElement T>
Matrix<T> operator*(T scalar, Matrix<T> matrix);

template <MatrixElement T>
Matrix<T> operator-(Matrix<T> matrix);

template <MatrixElement T>
Matrix<T> operator*(const Matrix<T>& left, const Matrix<T>& right);

template <MatrixElement T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix);

namespace matrix {
    template <MatrixElement T>
    Matrix<T> slow_multiply(const Matrix<T>& left, const Matrix<T>& right);

    template <MatrixElement T>
    Matrix<T> transpose(const Matrix<T>& matrix);

    template <MatrixElement T>
    bool approx_equal(
        const Matrix<T>& left,
        const Matrix<T>& right,
        T epsilon = T{100} * std::numeric_limits<T>::epsilon()
    );

    template <std::floating_point T>
    Matrix<T> solve(Matrix<T> A, Matrix<T> b);

    template <MatrixElement T>
    T maxValue(const Matrix<T>& matrix);

    template <MatrixElement T>
    T residual(const Matrix<T>& A, const Matrix<T>& b, const Matrix<T>& x);
}

#include "matrix_algorithms.tpp"
