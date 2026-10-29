template <MatrixElement T>
Matrix<T> operator+(Matrix<T> lhs, const Matrix<T>& rhs) {
    lhs += rhs;
    return lhs;
}

template <MatrixElement T>
Matrix<T> operator-(Matrix<T> lhs, const Matrix<T>& rhs) {
    lhs -= rhs;
    return lhs;
}

template <MatrixElement T>
Matrix<T> operator*(Matrix<T> matrix, T scalar) {
    matrix *= scalar;
    return matrix;
}

template <MatrixElement T>
Matrix<T> operator*(T scalar, Matrix<T> matrix) {
    matrix *= scalar;
    return matrix;
}

template <MatrixElement T>
Matrix<T> operator-(Matrix<T> matrix) {
    matrix *= T{-1};
    return matrix;
}

template <MatrixElement T>
Matrix<T> operator*(const Matrix<T>& left, const Matrix<T>& right) {
    const std::size_t left_cols = left.get_coloumn_count();
    const std::size_t left_rows = left.get_row_count();
    const std::size_t right_cols = right.get_coloumn_count();
    const std::size_t right_rows = right.get_row_count();

    if (left_cols != right_rows) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }

    Matrix<T> result(left_rows, right_cols);
    const T* left_data = left.elements.data();
    const T* right_data = right.elements.data();
    T* result_data = result.elements.data();
    for (std::size_t i = 0; i < left_rows; ++i) {
        for (std::size_t k = 0; k < left_cols; ++k) {
            const T value = left_data[i * left_cols + k];
            const T* right_row = &right_data[k * right_cols];
            T* result_row = &result_data[i * right_cols];
            for (std::size_t j = 0; j < right_cols; ++j) {
                result_row[j] += value * right_row[j];
            }
        }
    }
    return result;
}

namespace matrix {

template <MatrixElement T>
Matrix<T> slow_multiply(const Matrix<T>& left, const Matrix<T>& right) {
    const std::size_t left_cols = left.get_coloumn_count();
    const std::size_t left_rows = left.get_row_count();
    const std::size_t right_cols = right.get_coloumn_count();
    const std::size_t right_rows = right.get_row_count();

    if (left_cols != right_rows) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }

    Matrix<T> result(left_rows, right_cols);
    for (std::size_t i = 0; i < left_rows; ++i) {
        for (std::size_t j = 0; j < right_cols; ++j) {
            T sum = T{0};
            for (std::size_t k = 0; k < left_cols; ++k) {
                sum += left(i, k) * right(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

template <MatrixElement T>
Matrix<T> transpose(const Matrix<T>& matrix) {
    return matrix.transpose();
}

template <MatrixElement T>
bool approx_equal(const Matrix<T>& left, const Matrix<T>& right, T epsilon) {
    if (left.get_row_count() != right.get_row_count() ||
        left.get_coloumn_count() != right.get_coloumn_count()) {
        return false;
    }

    for (std::size_t i = 0; i < left.get_row_count(); ++i) {
        for (std::size_t j = 0; j < left.get_coloumn_count(); ++j) {
            if (std::abs(left(i, j) - right(i, j)) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

}

template <MatrixElement T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
    for (std::size_t i = 0; i < matrix.get_row_count(); ++i) {
        for (std::size_t j = 0; j < matrix.get_coloumn_count(); ++j) {
            os << matrix(i, j)
               << (j + 1 == matrix.get_coloumn_count() ? "" : " ");
        }
        os << '\n';
    }
    return os;
}

namespace matrix {

template <std::floating_point T>
Matrix<T> solve(Matrix<T> A, Matrix<T> b) {
    const std::size_t cols_a = A.get_coloumn_count();
    const std::size_t rows_a = A.get_row_count();
    const std::size_t cols_b = b.get_coloumn_count();
    const std::size_t rows_b = b.get_row_count();

    if (cols_a != rows_a) {
        throw std::invalid_argument("Matrix A is not square for Gaussian Elimination");
    }
    if (cols_b != 1) {
        throw std::invalid_argument("Matrix B is not a column vector for Gaussian Elimination");
    }
    if (rows_a != rows_b) {
        throw std::invalid_argument("Matrix row dimensions of A and B do not match for Gaussian elimination.");
    }

    const T scale = matrix::maxValue(A);
    const T tolerance = std::numeric_limits<T>::epsilon() * scale * static_cast<T>(rows_a);

    for (std::size_t i = 0; i < cols_a; ++i) {
        std::size_t pivot_row = i;
        T largest_pivot = std::abs(A(i, i));
        for (std::size_t row = i + 1; row < rows_a; ++row) {
            const T candidate = std::abs(A(row, i));
            if (candidate > largest_pivot) {
                largest_pivot = candidate;
                pivot_row = row;
            }
        }

        if (largest_pivot <= tolerance) {
            throw std::runtime_error("Matrix is singular or nearly singular, cannot solve.");
        }

        if (pivot_row != i) {
            for (std::size_t column = 0; column < cols_a; ++column) {
                std::swap(A(i, column), A(pivot_row, column));
            }
            std::swap(b(i, 0), b(pivot_row, 0));
        }

        const T pivot = A(i, i);
        for (std::size_t row = i + 1; row < rows_a; ++row) {
            const T factor = A(row, i) / pivot;
            A(row, i) = T{0};
            for (std::size_t column = i + 1; column < cols_a; ++column) {
                A(row, column) -= factor * A(i, column);
            }
            b(row, 0) -= factor * b(i, 0);
        }
    }

    Matrix<T> x(rows_a, 1);
    for (std::size_t step = 0; step < rows_b; ++step) {
        const std::size_t i = rows_b - 1 - step;
        if (std::abs(A(i, i)) <= tolerance) {
            throw std::runtime_error("Matrix is singular or nearly singular during back substitution, cannot solve.");
        }

        T sum = b(i, 0);
        for (std::size_t j = i + 1; j < rows_b; ++j) {
            sum -= A(i, j) * x(j, 0);
        }
        x(i, 0) = sum / A(i, i);
    }
    return x;
}

template <MatrixElement T>
T maxValue(const Matrix<T>& matrix) {
    T maximum = T{0};
    for (std::size_t i = 0; i < matrix.get_row_count(); ++i) {
        for (std::size_t j = 0; j < matrix.get_coloumn_count(); ++j) {
            maximum = std::max(maximum, std::abs(matrix(i, j)));
        }
    }
    return maximum;
}

template <MatrixElement T>
T residual(const Matrix<T>& A, const Matrix<T>& b, const Matrix<T>& x) {
    const Matrix<T> difference = b - (A * x);
    T maximum = T{0};
    for (std::size_t i = 0; i < difference.get_row_count(); ++i) {
        maximum = std::max(maximum, std::abs(difference(i, 0)));
    }
    return maximum;
}

}