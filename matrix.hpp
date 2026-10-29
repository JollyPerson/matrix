#pragma once

#include <cstddef>
#include <ostream>

template<typename T>
class Matrix {
private:
    std::size_t rows;
    std::size_t cols;
    T *elements;

    friend Matrix operator*(const Matrix &left, const Matrix &right);



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

    inline T operator()(size_t i, size_t j) const {
        return elements[i * cols + j];
    }

    T &at(size_t i, size_t j);

    T at(size_t i, size_t j) const;

    Matrix<T> &operator+=(const Matrix &other);

    Matrix<T> &operator*=(double scalar);

    Matrix<T> &operator-=(const Matrix &other);

    Matrix<T> &operator/=(double scalar);


    Matrix<T> transpose() const;

    void flush_zero() const;

    void swap(Matrix<T> &other) noexcept;


    std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;

    static Matrix<T> identity(size_t size);

    Matrix<T>(std::initializer_list<std::initializer_list<T>> list);
    
    void swap_rows(size_t row1, size_t row2);
};

template<typename T>
void swap(Matrix<T> &lhs, Matrix<T> &rhs) noexcept;

Matrix<T> operator+(Matrix<T> lhs, const Matrix<T> &rhs);

Matrix<T> operator-(Matrix<T> lhs, const Matrix<T> &rhs);

Matrix<T> operator*(Matrix<T> m, double scalar);

Matrix<T> operator*(double scalar, Matrix<T> m);

Matrix<T> operator-(Matrix<T> m);

Matrix slow_multiply(const Matrix &left, const Matrix &right);

Matrix transpose(const Matrix &m);

bool approx_equal(const Matrix &a, const Matrix &b, double epsilon = 1e-9);

std::ostream& operator<<(std::ostream& os, const Matrix& m);

//Gaussian Elimination to solve for x of Ax = B where A is a n square matrix and B is n x 1 matrix (n length vector) and x is the unknown matrix vector
Matrix solve(Matrix A, Matrix b);

double maxValue(const Matrix &m);

double residual(const Matrix &A, const Matrix &b, const Matrix &x);
