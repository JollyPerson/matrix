#include "matrix.hpp"
#include <stdexcept>
#include <assert.h>

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

