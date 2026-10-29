#include <algorithm>
#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <cmath>
#include <limits>

template<typename T>
Matrix<T>::Matrix(std::size_t row, std::size_t coloumn)
    : rows(row), cols(coloumn), elements(nullptr) {
    elements = new T[rows * cols]();
}

template<typename T>
Matrix<T>::~Matrix() {
    delete[] elements;
}
   
template<typename T>
Matrix<T>::Matrix(const Matrix<T> &old) : rows(old.rows), cols(old.cols) {
    const size_t size = rows * cols;
    elements = new T[size];
    for (size_t i = 0; i < size; i++) {
        elements[i] = old.elements[i];
    }
}

template<typename T>
Matrix<T>::Matrix(Matrix &&other) noexcept : rows(other.rows), cols(other.cols), elements(other.elements) {
    other.rows = 0;
    other.cols = 0;
    other.elements = nullptr;
}


template<typename T>
Matrix<T> &Matrix<T>::operator=(Matrix<T> &&other) noexcept {
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


template<typename T>
Matrix<T> &Matrix<T>::operator=(const Matrix<T> &old) {
    //Check if it equals itself
    if (this == &old) {
        return *this;
    }

    delete[] this->elements;
    this->rows = old.rows;
    this->cols = old.cols;
    size_t size = rows * cols;
    this->elements = new T[size];
    for (size_t i = 0; i < size; i++) {
        this->elements[i] = old.elements[i];
    }

    return *this;
}

template <typename T>
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

template <typename T>
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

template <typename T>
Matrix<T> &Matrix<T>::operator*=(const T scalar) {
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] *= scalar;
    }
    return *this;
}

template <typename T>
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

template <typename T>
std::size_t Matrix<T>::get_row_count() const {
    return rows;
}

template <typename T>
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
template <typename T>
T &Matrix<T>::at(std::size_t i, std::size_t j) {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i, j)];
}



//Will check at runtime if in bounds
template <typename T>
T Matrix<T>::at(std::size_t i, std::size_t j) const {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i, j)];
}

template <typename T>
void Matrix<T>::flush_zero() const {
    const size_t size = rows * cols;
    for (size_t i = 0; i < size; i++) {
        elements[i] = T{0};
    }
}

template <typename T>
void Matrix<T>::swap(Matrix<T> &other) noexcept {
    std::swap(this->elements, other.elements);
    std::swap(this->cols, other.cols);
    std::swap(this->rows, other.rows);
}

template <typename T>
Matrix<T> Matrix<T>::transpose() const {
    Matrix<T> result(cols,rows);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result(j,i) = (*this)(i,j);
        }

    }
    return result;
}

template <typename T>
Matrix<T> Matrix<T>::identity(size_t size) {
    Matrix<T> m(size,size);
    for (size_t i = 0; i < size; i++) {
        m(i,i) = T{1};
    }
    return m;
}

template <typename T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T> > list)
: rows(list.size()), cols(list.size() == 0 ? 0 :list.begin()->size()), elements(nullptr) {
    elements = new T[rows*cols]();
    size_t r = 0;
    for (const auto& row_list : list) {
        if (row_list.size() != cols) {
            delete[] elements;
            throw std::invalid_argument("All rows must have the same number of columns");
        }
        size_t c = 0;
        for (const T val: row_list) {
            elements[r*cols+c] = val;
            c++;
        }
        r++;
    }
}

template <typename T>
void swap(Matrix<T> &lhs, Matrix<T> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T>
Matrix<T> operator+(Matrix<T> lhs, const Matrix<T> &rhs) {
    lhs += rhs;
    return lhs;
}

template <typename T>
Matrix<T> operator-(Matrix<T> lhs, const Matrix<T> &rhs) {
    lhs -= rhs;
    return lhs;
}

template <typename T>
Matrix<T> operator*(Matrix<T> m, const T scalar) {
    m *= scalar;
    return m;
}

template <typename T>
Matrix<T> operator*(const T scalar, Matrix<T> m) {
    m *= scalar;
    return m;
}

template <typename T>
Matrix<T> operator-(Matrix<T> m) {
    m *= T{-1.0};
    return m;
}

template <typename T>
Matrix<T> operator*(const Matrix<T> &left, const Matrix<T> &right) {
    const size_t lc = left.get_coloumn_count();
    const size_t lr = left.get_row_count();

    const size_t rc = right.get_coloumn_count();
    const size_t rr = right.get_row_count();
    if (lc != rr) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }
    Matrix<T> result(lr,rc);

    const T* __restrict__ A = left.elements;
    const T* __restrict__ B = right.elements;
    T* __restrict__ C = result.elements;
    //Multiply correpsonding row and coloum and add the sum
    for (size_t i = 0; i < lr; ++i) {
        for (size_t k = 0; k < lc; ++k) {
            const T r  = A[i*lc+k];
            const T* __restrict__ B_row = &B[k * rc];
            T* __restrict__ C_row = &C[i * rc];

            for (size_t j = 0; j < rc; ++j) {
                C_row[j] += r * B_row[j];
            }
        }
    }

    return result;
}

template <typename T>
Matrix<T> slow_multiply(const Matrix<T> &left, const Matrix<T> &right) {
    const size_t lc = left.get_coloumn_count();
    const size_t lr = left.get_row_count();

    const size_t rc = right.get_coloumn_count();
    const size_t rr = right.get_row_count();

    if (lc != rr) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }
    Matrix<T> result(lr,rc);
    //Multiply correpsonding row and coloum and add the sum
    for (size_t i = 0; i < lr; ++i) {
        for (size_t j = 0; j < rc; ++j) {
            T sum = T{0};
            for (size_t k = 0; k < lc; ++k) {
                sum += left(i,k)*right(k,j);
            }
            result(i,j) = sum;
        }
    }

    return result;
}

template <typename T>
Matrix<T> transpose(const Matrix<T> &m) {
    return m.transpose();
}

template <typename T>
void Matrix<T>::swap_rows(size_t row1, size_t row2) {
    if (row1 >= rows || row2 >= rows) {
        throw std::out_of_range("Row index out of bounds for swap_rows");
    }
    for (size_t j = 0; j < cols; ++j) {
        std::swap((*this)(row1, j), (*this)(row2, j));
    }
}

template <typename T>
bool approx_equal(const Matrix<T> &a, const Matrix<T> &b, T epsilon) {
    for (size_t i = 0; i < a.get_row_count(); ++i) {
        for (size_t j = 0; j < a.get_coloumn_count(); ++j) {
            T x = a(i, j);
            T y = b(i, j);
            if (std::abs(x - y) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T> &m) {
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
template <std::floating_point T>
Matrix<T> solve(Matrix<T> A, Matrix<T> b) {
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
    const T scale = maxValue(A);
    const T tolerance =
        std::numeric_limits<T>::epsilon() *
        scale *
        static_cast<T>(rowA);
    //std::cout << "A before forward elimination:\n" << A << "\n";
    //std::cout << "b before forward elimination:\n" << b << "\n";
    //forward elimination to zero below the diag.
    //we need to go diagonal
    for (size_t i = 0; i < colA; ++i) {
        std::size_t pivot_row = i;
        T largest_pivot = std::abs(A(i, i));
        for(size_t row = i+1; row<rowA; ++row){
            const T candidate = std::abs(A(row, i));
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

        const T pivot = A(i, i);
        for (size_t j = i + 1; j < rowA; ++j) {
            const T factor = A(j, i) / pivot;

            // Explicitly zero the cleared entry to avoid floating-point residual noise (e.g. 1e-17)
            A(j, i) = T{0};

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

    Matrix<T> x(rowA, 1);

    for (size_t step = 0; step < rowB; ++step) {
        size_t i = (rowB - 1) - step;

        if (std::abs(A(i, i)) <= tolerance) {
            throw std::runtime_error("Matrix is singular or nearly singular during back substitution, cannot solve.");
        }

        T sum = b(i, 0);
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

template <typename T>
T maxValue(const Matrix<T> &m) {
    T max_val = T{0};
    for (size_t i = 0; i < m.get_row_count(); ++i) {
        for (size_t j = 0; j < m.get_coloumn_count(); ++j) {
            max_val = std::max(max_val, std::abs(m(i, j)));
        }
    }
    return max_val;
}

template <typename T>
T residual(const Matrix<T> &A, const Matrix<T> &b, const Matrix<T> &x){
    //r = b - Ax
    Matrix<T> difference = b - (A * x);
    T maximum = T{0};
    size_t rows = difference.get_row_count();
    for(size_t i =0; i<rows; ++i){
        maximum = std::max(maximum, std::abs(difference(i,0)));
    }
    return maximum;
}