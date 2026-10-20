#include "matrix.hpp"
#include <algorithm>
#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <cmath>
#include <limits>

Matrix::Matrix(std::size_t row, std::size_t coloumn)
    : rows(row), cols(coloumn), elements(nullptr) {
    elements = new double[rows * cols]();
}

Matrix::~Matrix() {
    delete[] elements;
}


//Will break if rows*cols is > size_t max i.e (size_t_max / rows) < cols
Matrix::Matrix(const Matrix &old) : rows(old.rows), cols(old.cols) {
    const size_t size = rows * cols;
    elements = new double[size];
    for (size_t i = 0; i < size; i++) {
        elements[i] = old.elements[i];
    }
}

Matrix::Matrix(Matrix &&other) noexcept : rows(other.rows), cols(other.cols), elements(other.elements) {
    other.rows = 0;
    other.cols = 0;
    other.elements = nullptr;
}

Matrix &Matrix::operator=(Matrix &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    delete[] elements;

    rows = other.rows;
    cols = other.cols;
    elements = other.elements;

    other.rows = 0;
    other.cols = 0;
    other.elements = nullptr;

    return *this;
}


Matrix &Matrix::operator=(const Matrix &old) {
    //Check if it equals itself
    if (this == &old) {
        return *this;
    }

    delete[] this->elements;
    this->rows = old.rows;
    this->cols = old.cols;
    size_t size = rows * cols;
    this->elements = new double[size];
    for (size_t i = 0; i < size; i++) {
        this->elements[i] = old.elements[i];
    }

    return *this;
}

Matrix &Matrix::operator+=(const Matrix &other) {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] += other.elements[i];
    }
    return *this;
}

Matrix &Matrix::operator-=(const Matrix &other) {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] -= other.elements[i];
    }
    return *this;
}

Matrix &Matrix::operator*=(const double scalar) {
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] *= scalar;
    }
    return *this;
}

Matrix &Matrix::operator/=(const double scalar) {
    if (scalar == 0.0) {
        throw std::invalid_argument("Division by zero is not allowed");
    }
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] /= scalar;
    }
    return *this;
}

std::size_t Matrix::get_row_count() const {
    return rows;
}

std::size_t Matrix::get_coloumn_count() const {
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
double &Matrix::at(std::size_t i, std::size_t j) {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i, j)];
}


//Will check at runtime if in bounds
double Matrix::at(std::size_t i, std::size_t j) const {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i, j)];
}

void Matrix::flush_zero() const {
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] = 0.0;
    }
}

void Matrix::swap(Matrix &other) noexcept {
    std::swap(this->elements, other.elements);
    std::swap(this->cols, other.cols);
    std::swap(this->rows, other.rows);
}

Matrix Matrix::transpose() const {
    Matrix result(cols,rows);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result(j,i) = (*this)(i,j);
        }

    }
    return result;
}

Matrix Matrix::identity(size_t size) {
    Matrix m(size,size);
    for (size_t i = 0; i < size; i++) {
        m(i,i) = 1.0;
    }
    return m;
}

Matrix::Matrix(std::initializer_list<std::initializer_list<double> > list)
: rows(list.size()), cols(list.size() == 0 ? 0 :list.begin()->size()), elements(nullptr) {
    elements = new double[rows*cols]();
    size_t r = 0;
    for (const auto& row_list : list) {
        if (row_list.size() != cols) {
            delete[] elements;
            throw std::invalid_argument("All rows must have the same number of columns");
        }
        size_t c = 0;
        for (const double val: row_list) {
            elements[r*cols+c] = val;
            c++;
        }
        r++;
    }
}

void swap(Matrix &lhs, Matrix &rhs) noexcept {
    lhs.swap(rhs);
}

Matrix operator+(Matrix lhs, const Matrix &rhs) {
    lhs += rhs;
    return lhs;
}

Matrix operator-(Matrix lhs, const Matrix &rhs) {
    lhs -= rhs;
    return lhs;
}

Matrix operator*(Matrix m, const double scalar) {
    m *= scalar;
    return m;
}

Matrix operator*(const double scalar, Matrix m) {
    m *= scalar;
    return m;
}

Matrix operator-(Matrix m) {
    m *= -1.0;
    return m;
}

Matrix operator*(const Matrix &left, const Matrix &right) {
    const size_t lc = left.get_coloumn_count();
    const size_t lr = left.get_row_count();

    const size_t rc = right.get_coloumn_count();
    const size_t rr = right.get_row_count();
    if (lc != rr) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }
    Matrix result(lr,rc);

    const double* __restrict__ A = left.elements;
    const double* __restrict__ B = right.elements;
    double* __restrict__ C = result.elements;
    //Multiply correpsonding row and coloum and add the sum
    for (size_t i = 0; i < lr; ++i) {
        for (size_t k = 0; k < lc; ++k) {
            const double r  = A[i*lc+k];
            const double* __restrict__ B_row = &B[k * rc];
            double* __restrict__ C_row = &C[i * rc];

            for (size_t j = 0; j < rc; ++j) {
                C_row[j] += r * B_row[j];
            }
        }
    }

    return result;
}

Matrix slow_multiply(const Matrix &left, const Matrix &right) {
    const size_t lc = left.get_coloumn_count();
    const size_t lr = left.get_row_count();

    const size_t rc = right.get_coloumn_count();
    const size_t rr = right.get_row_count();

    if (lc != rr) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }
    Matrix result(lr,rc);
    //Multiply correpsonding row and coloum and add the sum
    for (size_t i = 0; i < lr; ++i) {
        for (size_t j = 0; j < rc; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < lc; ++k) {
                sum += left(i,k)*right(k,j);
            }
            result(i,j) = sum;
        }
    }

    return result;
}

Matrix transpose(const Matrix &m) {
    return m.transpose();
}

bool approx_equal(const Matrix &a, const Matrix &b, double epsilon) {
    for (size_t i = 0; i < a.get_row_count(); ++i) {
        for (size_t j = 0; j < a.get_coloumn_count(); ++j) {
            double x = a(i, j);
            double y = b(i, j);
            if (std::abs(x - y) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Matrix &m) {
    const size_t rows = m.get_row_count();
    const size_t cols = m.get_coloumn_count();

    for (size_t i = 0; i<rows; ++i) {
        for (size_t j=0; j<cols;++j) {
            os << m(i,j) << (j+1 == cols ? "" : " ");
        }
        os << '\n';
    }
    return os;
}

//Gaussian Elimination to solve for x of Ax = B where A is a n square matrix and B is n x 1 matrix (n length vector) and x is the unknown matrix vector
Matrix solve(Matrix A, Matrix b) {
    auto colA = A.get_coloumn_count();
    auto rowA = A.get_row_count();
    auto colB = b.get_coloumn_count();
    auto rowB = b.get_row_count();

    if (colA != rowA) {
        throw std::invalid_argument("Matrix A is not square for Gaussian Elimination");
    }
    if (colB != 1) {
        throw std::invalid_argument("Matrix B is not a column vector for Gaussian Elimination");
    }
    if (rowA != rowB) {
        throw std::invalid_argument("Matrix row dimensions of A and B do not match for Gaussian elimination.");
    }
    const double scale = maxValue(A);
    const double tolerance =
        std::numeric_limits<double>::epsilon() *
        scale *
        static_cast<double>(rowA);
    //std::cout << "A before forward elimination:\n" << A << "\n";
    //std::cout << "b before forward elimination:\n" << b << "\n";
    //forward elimination to zero below the diag.
    //we need to go diagonal
    for (size_t i = 0; i < colA; ++i) {
        std::size_t pivot_row = i;
        double largest_pivot = std::abs(A(i, i));
        for(size_t row = i+1; row<rowA; ++row){
            const double candidate = std::abs(A(row, i));
            if (candidate > largest_pivot) {
                largest_pivot = candidate;
                pivot_row = row;
            }
        }
        if(largest_pivot <=tolerance){
            throw std::runtime_error("Matrix is singular or nearly singular, cannot solve.");
        }

        if(pivot_row != i){
            for(std::size_t column = 0; column < colA; ++column){
                std::swap(A(i, column), A(pivot_row, column));
            }
            std::swap(b(i, 0), b(pivot_row, 0));
        }

        const double pivot = A(i, i);
        for (size_t j = i + 1; j < rowA; ++j) {
            const double factor = A(j, i) / pivot;

            // Explicitly zero the cleared entry to avoid floating-point residual noise (e.g. 1e-17)
            A(j, i) = 0.0;

            // Eliminate remaining columns in row j
            for (size_t k = i + 1; k < colA; ++k) {
                A(j, k) -= factor * A(i, k);
            }

            // Apply the same operation to vector b
            b(j, 0) -= factor * b(i, 0);
        }
    }
    //std::cout << "A after forward elimination:\n" << A << "\n";
    //std::cout << "b after forward elimination:\n" << b << "\n";

    Matrix x(rowA, 1);

    for (size_t step = 0; step < rowB; ++step) {
        size_t i = (rowB - 1) - step;

        if (std::abs(A(i, i)) <= tolerance) {
            throw std::runtime_error("Matrix is singular or nearly singular during back substitution, cannot solve.");
        }

        double sum = b(i, 0);
        for (size_t j = i + 1; j < rowB; ++j) {
            sum -= A(i, j) * x(j, 0);
        }
        x(i, 0) = sum / A(i, i);
    }
    //std::cout << "x after back substitution:\n" << x << "\n";
    //std::cout << "A after back substitution:\n" << A << "\n";
    //std::cout << "B after back substitution:\n" << b << "\n";
    return x;
}

double maxValue(const Matrix &m) {
    double max_val = 0.0;
    for (size_t i = 0; i < m.get_row_count(); ++i) {
        for (size_t j = 0; j < m.get_coloumn_count(); ++j) {
            max_val = std::max(max_val, std::abs(m(i, j)));
        }
    }
    return max_val;
}

double residual(const Matrix &A, const Matrix &b, const Matrix &x){
    //r = b - Ax
    Matrix difference = b - (A * x);
    double maximum = 0;
    size_t rows = difference.get_row_count();
    for(size_t i =0; i<rows; ++i){
        maximum = std::max(maximum, std::abs(difference(i,0)));
    }
    return maximum;
}