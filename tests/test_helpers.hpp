#pragma once

#include <cassert>
#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "../lu.hpp"
#include "../matrix.hpp"
#include "../raw_array_matrix.hpp"

static_assert(MatrixElement<int>);
static_assert(MatrixElement<float>);
static_assert(MatrixElement<double>);
static_assert(!MatrixElement<bool>);
static_assert(!MatrixElement<std::string>);

struct CopyCountingNumber {
    static inline std::size_t copies = 0;
    static inline std::size_t moves = 0;

    int value{};

    CopyCountingNumber() = default;
    CopyCountingNumber(int value) : value(value) {}

    CopyCountingNumber(const CopyCountingNumber& other) : value(other.value) {
        ++copies;
    }

    CopyCountingNumber(CopyCountingNumber&& other) noexcept : value(other.value) {
        ++moves;
    }

    CopyCountingNumber& operator=(const CopyCountingNumber& other) {
        value = other.value;
        ++copies;
        return *this;
    }

    CopyCountingNumber& operator=(CopyCountingNumber&& other) noexcept {
        value = other.value;
        ++moves;
        return *this;
    }

    CopyCountingNumber& operator+=(const CopyCountingNumber& other) {
        value += other.value;
        return *this;
    }

    CopyCountingNumber& operator-=(const CopyCountingNumber& other) {
        value -= other.value;
        return *this;
    }

    CopyCountingNumber& operator*=(const CopyCountingNumber& other) {
        value *= other.value;
        return *this;
    }

    CopyCountingNumber& operator/=(const CopyCountingNumber& other) {
        value /= other.value;
        return *this;
    }
};

inline CopyCountingNumber operator+(CopyCountingNumber lhs, const CopyCountingNumber& rhs) {
    lhs += rhs;
    return lhs;
}

inline CopyCountingNumber operator-(CopyCountingNumber lhs, const CopyCountingNumber& rhs) {
    lhs -= rhs;
    return lhs;
}

inline CopyCountingNumber operator*(CopyCountingNumber lhs, const CopyCountingNumber& rhs) {
    lhs *= rhs;
    return lhs;
}

inline CopyCountingNumber operator/(CopyCountingNumber lhs, const CopyCountingNumber& rhs) {
    lhs /= rhs;
    return lhs;
}

inline CopyCountingNumber operator-(CopyCountingNumber value) {
    value.value = -value.value;
    return value;
}

inline bool operator==(const CopyCountingNumber& lhs, const CopyCountingNumber& rhs) {
    return lhs.value == rhs.value;
}

static_assert(MatrixElement<CopyCountingNumber>);

template <typename T>
concept has_matrix_solver = requires(Matrix<T> A, Matrix<T> b) {
    matrix::solve(A, b);
};

static_assert(!has_matrix_solver<int>,
              "solve must reject integer matrices at compile time");

// ==========================================
// Benchmarking & Generation Helpers
// ==========================================

template <typename T>
double sum_matrix_row(const Matrix<T> &matrix) {
    double sum = 0;
    size_t rows = matrix.get_row_count();
    size_t cols = matrix.get_coloumn_count();
    for (size_t x = 0; x < rows; ++x) {
        for (size_t y = 0; y < cols; ++y) {
            sum += matrix(x, y);
        }
    }
    return sum;
}

template <typename T>
double sum_matrix_coloumn(const Matrix<T> &matrix) {
    double sum = 0;
    size_t rows = matrix.get_row_count();
    size_t cols = matrix.get_coloumn_count();
    for (size_t y = 0; y < cols; ++y) {
        for (size_t x = 0; x < rows; ++x) {
            sum += matrix(x, y);
        }
    }
    return sum;
}

template <typename T>
Matrix<T> generate_matrix(int seed, size_t rows, size_t cols, T min = T{0}, T max = T{1000}) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<T> distr(min, max);
    Matrix<T> matrix(rows, cols);
    for (size_t x = 0; x < rows; ++x) {
        for (size_t y = 0; y < cols; ++y) {
            matrix(x, y) = distr(gen);
        }
    }
    return matrix;
}

template <typename T>
T test_tolerance() {
    return T{100} * std::numeric_limits<T>::epsilon();
}

template <typename T>
void test_reusable_construction_and_ownership() {
    Matrix<T> matrix(2, 3);
    assert(matrix.get_row_count() == 2);
    assert(matrix.get_coloumn_count() == 3);
    assert(matrix(0, 0) == T{0});

    matrix(1, 2) = T{7};
    Matrix<T> copy = matrix;
    copy(1, 2) = T{9};
    assert(matrix(1, 2) == T{7});
    assert(copy(1, 2) == T{9});

    Matrix<T> moved = std::move(copy);
    assert(moved(1, 2) == T{9});
    assert(copy.get_row_count() == 2);
    assert(copy.get_coloumn_count() == 3);
}

template <typename T>
void test_reusable_arithmetic() {
    const Matrix<T> a{
        {T{1}, T{2}},
        {T{3}, T{4}}
    };
    const Matrix<T> b{
        {T{5}, T{6}},
        {T{7}, T{8}}
    };

    const T epsilon = test_tolerance<T>();
    assert(matrix::approx_equal(a + b, Matrix<T>{{T{6}, T{8}}, {T{10}, T{12}}}, epsilon));
    assert(matrix::approx_equal((a + b) - b, a, epsilon));
    assert(matrix::approx_equal(T{2} * a, a + a, epsilon));
    assert(matrix::approx_equal(a.transpose().transpose(), a, epsilon));
    assert(matrix::approx_equal(a * b,
                        Matrix<T>{{T{19}, T{22}}, {T{43}, T{50}}},
                        epsilon));
    assert(matrix::approx_equal(Matrix<T>::identity(2) * a, a, epsilon));
}

template <std::floating_point T>
void test_reusable_solver() {
    const Matrix<T> a{
        {T{3}, T{1}},
        {T{1}, T{2}}
    };
    const Matrix<T> b{
        {T{9}},
        {T{8}}
    };

    const Matrix<T> x = matrix::solve(a, b);
    assert(matrix::residual(a, b, x) < T{1000} * std::numeric_limits<T>::epsilon());
}

template <typename T>
void run_reusable_tests() {
    test_reusable_construction_and_ownership<T>();
    test_reusable_arithmetic<T>();
    if constexpr (std::floating_point<T>) {
        test_reusable_solver<T>();
    }
}
