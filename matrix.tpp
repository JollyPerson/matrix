#include <algorithm>
#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <cmath>
#include <limits>

template <MatrixElement T>
Matrix<T>::Matrix(std::size_t row, std::size_t coloumn)
    : rows(row), cols(coloumn), elements(rows * cols) {
#ifndef NDEBUG
    assert_invariant();
#endif
}

#ifndef NDEBUG
template <MatrixElement T>
void Matrix<T>::assert_invariant() const noexcept {
    assert(elements.empty() || elements.size() == rows * cols);
}
#endif

template <MatrixElement T>
Matrix<T> &Matrix<T>::operator+=(const Matrix<T> &other) {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] += other.elements[i];
    }
    return *this;
}

template <MatrixElement T>
Matrix<T> &Matrix<T>::operator-=(const Matrix<T> &other) {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] -= other.elements[i];
    }
    return *this;
}

template <MatrixElement T>
Matrix<T> &Matrix<T>::operator*=(const T scalar) {
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] *= scalar;
    }
    return *this;
}

template <MatrixElement T>
Matrix<T> &Matrix<T>::operator/=(const T scalar) {
    if (scalar == T{0.0}) {
        throw std::invalid_argument("Division by zero is not allowed");
    }
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] /= scalar;
    }
    return *this;
}

template <MatrixElement T>
std::size_t Matrix<T>::get_row_count() const {
    return rows;
}

template <MatrixElement T>
std::size_t Matrix<T>::get_coloumn_count() const {
    return cols;
}
/**
std::size_t Matrix::offset(size_t x, size_t y) const {
    //Row Major offset
    return x * cols + y;
}

//RW with assertions for debugging, will inline when production mode
double &Matrix::operator()(size_t i, size_t j) {
    assert(i < rows && "Row index out of bounds");
    assert(j < cols && "Coloumn index of out of bounds");
    return elements[offset(i, j)];
}

//R will inline for const Matrix&
double Matrix::operator()(size_t i, size_t j) const {
    assert(i < rows && "Row index out of bounds");
    assert(j < cols && "Coloumn index of out of bounds");
    return elements[offset(i, j)];
}
**/

//will check at runtime if in bounds
template <MatrixElement T>
T &Matrix<T>::at(std::size_t i, std::size_t j) {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i, j)];
}



//Will check at runtime if in bounds
template <MatrixElement T>
T Matrix<T>::at(std::size_t i, std::size_t j) const {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i, j)];
}

template <MatrixElement T>
void Matrix<T>::flush_zero() {
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] = T{0};
    }
}

template <MatrixElement T>
void Matrix<T>::swap(Matrix<T> &other) noexcept {
    std::swap(this->elements, other.elements);
    std::swap(this->cols, other.cols);
    std::swap(this->rows, other.rows);
#ifndef NDEBUG
    assert_invariant();
    other.assert_invariant();
#endif
}

template <MatrixElement T>
Matrix<T> Matrix<T>::transpose() const {
    Matrix<T> result(cols,rows);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result(j,i) = (*this)(i,j);
        }

    }
    return result;
}

template <MatrixElement T>
Matrix<T> Matrix<T>::identity(size_t size) {
    Matrix<T> m(size,size);
    for (size_t i = 0; i < size; i++) {
        m(i,i) = T{1};
    }
    return m;
}

template <MatrixElement T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T> > list)
: rows(list.size()), cols(list.size() == 0 ? 0 :list.begin()->size()), elements(rows * cols) {
    size_t r = 0;
    for (const auto& row_list : list) {
        if (row_list.size() != cols) {
            throw std::invalid_argument("All rows must have the same number of columns");
        }
        size_t c = 0;
        for (const T& val: row_list) {
            elements[r*cols+c] = val;
            c++;
        }
        r++;
    }
#ifndef NDEBUG
    assert_invariant();
#endif
}

template <MatrixElement T>
void swap(Matrix<T> &lhs, Matrix<T> &rhs) noexcept {
    lhs.swap(rhs);
}

template <MatrixElement T>
void Matrix<T>::swap_rows(size_t row1, size_t row2) {
    if (row1 >= rows || row2 >= rows) {
        throw std::out_of_range("Row index out of bounds for swap_rows");
    }
    for (size_t j = 0; j < cols; ++j) {
        std::swap((*this)(row1, j), (*this)(row2, j));
    }
}