#pragma once

#include <cstddef>
#include <ostream>
#include <concepts>
#include <initializer_list>
#include <limits>

template<typename T>
class Matrix;

template<typename T>
Matrix<T> operator*(const Matrix<T>& left, const Matrix<T>& right);

template<typename T = double>
class Matrix {
private:
    std::size_t rows;
    std::size_t cols;
    T *elements;

    friend Matrix<T> operator*<>(const Matrix<T> &left, const Matrix<T> &right);


public:
    Matrix(std::size_t row, std::size_t col);

    //Rule of 5: Destructor, Copy Constructor, Copy Assignment, Move Constructor, Move Assignment
    //Desturctor
    ~Matrix();

    //Copy constructor
    Matrix(const Matrix &old);

    //Copy Assignment
    Matrix<T> &operator=(const Matrix &old);

    //Move constructor (accepts a rvalue)
    Matrix(Matrix &&other) noexcept;

    //Move assignment (accepts a rvalue)
    Matrix<T> &operator=(Matrix &&other) noexcept;

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

    void flush_zero() const;

    void swap(Matrix<T> &other) noexcept;


    std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;

    static Matrix<T> identity(size_t size);

    Matrix(std::initializer_list<std::initializer_list<T>> list);
    
    void swap_rows(size_t row1, size_t row2);
};

template<typename T>
void swap(Matrix<T> &lhs, Matrix<T> &rhs) noexcept;

template <typename T>
Matrix<T> operator+(Matrix<T> lhs, const Matrix<T> &rhs);

template <typename T>
Matrix<T> operator-(Matrix<T> lhs, const Matrix<T> &rhs);

template <typename T>
Matrix<T> operator*(Matrix<T> m, T scalar);

template <typename T>
Matrix<T> operator*(T scalar, Matrix<T> m);

template <typename T>
Matrix<T> operator-(Matrix<T> m);

template <typename T>
Matrix<T> slow_multiply(const Matrix<T> &left, const Matrix<T> &right);

template <typename T>
Matrix<T> transpose(const Matrix<T> &m);

template <typename T>
bool approx_equal(const Matrix<T> &a, const Matrix<T> &b, T epsilon = T{100} * std::numeric_limits<T>::epsilon());

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m);

//Gaussian Elimination to solve for x of Ax = B where A is a n square matrix and B is n x 1 matrix (n length vector) and x is the   unknown matrix vector
template <std::floating_point T>
Matrix<T> solve(Matrix<T> A, Matrix<T> b);

template <typename T>
T maxValue(const Matrix<T> &m);

template <typename T>
T residual(const Matrix<T> &A, const Matrix<T> &b, const Matrix<T> &x);

#include "matrix.tpp"
