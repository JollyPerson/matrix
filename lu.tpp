#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

template <std::floating_point T>
matrix::LUDecomposition<T>::LUDecomposition(const Matrix<T>& A)
    : factors(A),
      permutation(A.get_row_count()),
      swap_count(0) {
    const std::size_t n = A.get_row_count();
    if (n == 0) {
        throw std::invalid_argument("LU decomposition requires a non-empty matrix");
    }
    if (A.get_coloumn_count() != n) {
        throw std::invalid_argument("LU decomposition requires a square matrix");
    }
    std::iota(permutation.begin(), permutation.end(), 0);
    factor();
}

template <std::floating_point T>
Matrix<T> matrix::LUDecomposition<T>::factor() {
    const std::size_t n = factors.get_row_count();
    const T scale = matrix::maxValue(factors);
    const T tolerance = std::numeric_limits<T>::epsilon() * scale * static_cast<T>(n);

    for (std::size_t k = 0; k < n; ++k) {
        std::size_t pivot_row = k;
        T largest = std::abs(factors(k, k));
        for (std::size_t row = k + 1; row < n; ++row) {
            const T candidate = std::abs(factors(row, k));
            if (candidate > largest) {
                largest = candidate;
                pivot_row = row;
            }
        }
        if (largest < tolerance) {
            throw std::runtime_error("Matrix is singular to working precision.");
        }
        if (pivot_row != k) {
            std::swap(permutation[k], permutation[pivot_row]);
            factors.swap_rows(k, pivot_row);
            ++swap_count;
        }
        for (std::size_t row = k + 1; row < n; ++row) {
            const T multiplier = factors(row, k) / factors(k, k);
            factors(row, k) = multiplier;
            for (std::size_t column = k + 1; column < n; ++column) {
                factors(row, column) -= multiplier * factors(k, column);
            }
        }
    }
    return factors;
}

template <std::floating_point T>
Matrix<T> matrix::LUDecomposition<T>::solve(const Matrix<T>& b) const {
    const std::size_t row_b = b.get_row_count();
    const std::size_t col_b = b.get_coloumn_count();
    const std::size_t factor_rows = factors.get_row_count();
    if (row_b != factor_rows || col_b != 1) {
        throw std::invalid_argument("Matrix B dimensions do not match for solving.");
    }

    Matrix<T> y(factor_rows, 1);
    for (std::size_t i = 0; i < factor_rows; ++i) {
        y(i, 0) = b(permutation[i], 0);
    }
    for (std::size_t i = 0; i < factor_rows; ++i) {
        T sum = y(i, 0);
        for (std::size_t j = 0; j < i; ++j) {
            sum -= factors(i, j) * y(j, 0);
        }
        y(i, 0) = sum;
    }

    Matrix<T> x(factor_rows, 1);
    for (std::size_t step = 0; step < factor_rows; ++step) {
        const std::size_t i = factor_rows - 1 - step;
        T sum = y(i, 0);
        for (std::size_t j = i + 1; j < factor_rows; ++j) {
            sum -= factors(i, j) * x(j, 0);
        }
        x(i, 0) = sum / factors(i, i);
    }
    return x;
}

template <std::floating_point T>
const Matrix<T>& matrix::LUDecomposition<T>::get_factors() const {
    return factors;
}

template <std::floating_point T>
const std::vector<std::size_t>& matrix::LUDecomposition<T>::get_permutation() const {
    return permutation;
}

template <std::floating_point T>
std::size_t matrix::LUDecomposition<T>::get_swap_count() const {
    return swap_count;
}

template <std::floating_point T>
T matrix::LUDecomposition<T>::determinant() const {
    T determinant = (swap_count % 2 == 0) ? T{1} : T{-1};
    for (std::size_t i = 0; i < factors.get_row_count(); ++i) {
        determinant *= factors(i, i);
    }
    return determinant;
}

template <std::floating_point T>
Matrix<T> matrix::LUDecomposition<T>::inverse() const {
    const std::size_t n = factors.get_row_count();
    Matrix<T> inverse_matrix(n, n);
    for (std::size_t column = 0; column < n; ++column) {
        Matrix<T> unit_vector(n, 1);
        unit_vector(column, 0) = T{1};
        const Matrix<T> solution = solve(unit_vector);
        for (std::size_t row = 0; row < n; ++row) {
            inverse_matrix(row, column) = solution(row, 0);
        }
    }
    return inverse_matrix;
}
