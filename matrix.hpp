#pragma once

#include <cstddef>
#include <ostream>

class Matrix {
private:
    std::size_t rows;
    std::size_t cols;
    double *elements;

    friend Matrix operator*(const Matrix &left, const Matrix &right);



public:
    Matrix(std::size_t row, std::size_t col);

    //Rule of 5: Destructor, Copy Constructor, Copy Assignment, Move Constructor, Move Assignment
    //Desturctor
    ~Matrix();

    //Copy constructor
    Matrix(const Matrix &old);

    //Copy Assignment
    Matrix &operator=(const Matrix &old);

    //Move constructor (accepts a rvalue)
    Matrix(Matrix &&other) noexcept;

    //Move assignment (accepts a rvalue)
    Matrix &operator=(Matrix &&other) noexcept;

    inline std::size_t offset(size_t x, size_t y) const {
        return x * cols + y;
    }

    inline double& operator()(size_t i, size_t j) {
        return elements[i * cols + j]; // inlined directly to a single pointer offset!
    }

    inline double operator()(size_t i, size_t j) const {
        return elements[i * cols + j];
    }

    double &at(size_t i, size_t j);

    double at(size_t i, size_t j) const;

    Matrix &operator+=(const Matrix &other);

    Matrix &operator*=(double scalar);

    Matrix &operator-=(const Matrix &other);

    Matrix &operator/=(double scalar);


    Matrix transpose() const;

    void flush_zero() const;

    void swap(Matrix &other) noexcept;


    std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;

    static Matrix identity(size_t size);

    Matrix(std::initializer_list<std::initializer_list<double>> list);
};

void swap(Matrix &lhs, Matrix &rhs) noexcept;

Matrix operator+(Matrix lhs, const Matrix &rhs);

Matrix operator-(Matrix lhs, const Matrix &rhs);

Matrix operator*(Matrix m, double scalar);

Matrix operator*(double scalar, Matrix m);

Matrix operator-(Matrix m);

Matrix slow_multiply(const Matrix &left, const Matrix &right);

Matrix transpose(const Matrix &m);

bool approx_equal(const Matrix &a, const Matrix &b, double epsilon = 1e-9);

std::ostream& operator<<(std::ostream& os, const Matrix& m);

//Gaussian Elimination to solve for x of Ax = B where A is a n square matrix and B is n x 1 matrix (n length vector) and x is the unknown matrix vector
Matrix solve(const Matrix &A, const Matrix &b);

