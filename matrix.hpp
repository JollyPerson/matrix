#pragma once

#include <cstddef>
#include <ostream>
#include <concepts>
#include <initializer_list>
#include <limits>
#include <vector>

template <typename T>
concept MatrixElement =
    (std::integral<T> || std::floating_point<T>) &&
    !std::same_as<T, bool>;

template <MatrixElement T>
class Matrix;

template <MatrixElement T>
Matrix<T> operator*(const Matrix<T>& left, const Matrix<T>& right);

template <MatrixElement T = double>
class Matrix {
private:
    std::size_t rows;
    std::size_t cols;
    std::vector<T> elements;

    friend Matrix<T> operator*<>(const Matrix<T> &left, const Matrix<T> &right);


public:
    Matrix(std::size_t row, std::size_t col);

    inline std::size_t offset(size_t x, size_t y) const {
        return x * cols + y;
    }

    inline T& operator()(size_t i, size_t j) {
        return elements[i * cols + j]; // inlined directly to a single pointer offset!
    }

    inline const T& operator()(size_t i, size_t j) const {
        return elements[i * cols + j];
    }

    T &at(size_t i, size_t j);

    T at(size_t i, size_t j) const;

    Matrix<T> &operator+=(const Matrix &other);

    Matrix<T> &operator*=(T scalar);

    Matrix<T> &operator-=(const Matrix &other);

    Matrix<T> &operator/=(T scalar);


    Matrix<T> transpose() const;

    void flush_zero();

    void swap(Matrix<T> &other) noexcept;


    std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;

    static Matrix<T> identity(size_t size);

    Matrix(std::initializer_list<std::initializer_list<T>> list);
    
    void swap_rows(size_t row1, size_t row2);
};

template<typename T>
void swap(Matrix<T> &lhs, Matrix<T> &rhs) noexcept;

#include "matrix.tpp"
#include "matrix_algorithms.hpp"
