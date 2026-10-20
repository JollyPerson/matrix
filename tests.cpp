#include <cassert>
#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "matrix.hpp"

// ==========================================
// Benchmarking & Generation Helpers
// ==========================================

double sum_matrix_row(const Matrix &matrix) {
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

double sum_matrix_coloumn(const Matrix &matrix) {
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

Matrix generate_matrix(int seed, size_t rows, size_t cols, double min = 0, double max = 1000) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> distr(min, max);
    Matrix matrix(rows, cols);
    for (size_t x = 0; x < rows; ++x) {
        for (size_t y = 0; y < cols; ++y) {
            matrix(x, y) = distr(gen);
        }
    }
    return matrix;
}

void test_sum_speed() {
    auto start = std::chrono::high_resolution_clock::now();
    const Matrix a = generate_matrix(42, 4000, 4000);
    const Matrix b = generate_matrix(43, 4000, 4000);

    // 1. Row-by-row traversal (Stride 1 - Fast)
    auto t0 = std::chrono::high_resolution_clock::now();
    double row_sum = sum_matrix_row(a);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto row_duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    // 2. Col-by-col traversal (Stride cols - Slow)
    auto t2 = std::chrono::high_resolution_clock::now();
    double col_sum = sum_matrix_coloumn(b);
    auto t3 = std::chrono::high_resolution_clock::now();
    auto col_duration = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);

    auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(t3 - start);

    std::cout << "Sum speed benchmark:\n";
    std::cout << "  Row-by-row (stride 1)    : " << row_duration.count() << " micros (sum = " << row_sum << ")\n";
    std::cout << "  Col-by-col (stride cols) : " << col_duration.count() << " micros (sum = " << col_sum << ")\n";
    std::cout << "  Total                    : " << total_duration.count() << " micros\n";
    double speedup = row_duration.count() > 0 ? static_cast<double>(col_duration.count()) / static_cast<double>(row_duration.count()) : 0.0;

    std::cout << "  Speedup                  : " << std::fixed << std::setprecision(2) << speedup << "x\n";

}

// ==========================================
// Constuctors & Element Access
// ==========================================

void test_dimensions_and_access() {
    Matrix empty(0, 0);
    assert(empty.get_row_count() == 0);
    assert(empty.get_coloumn_count() == 0);

    Matrix square(2, 2);
    assert(square.get_row_count() == 2);
    assert(square.get_coloumn_count() == 2);
    assert(square(0, 0) == 0.0); // Verifies zero-initialization

    Matrix rectangular(5, 9);
    assert(rectangular.get_row_count() == 5);
    assert(rectangular.get_coloumn_count() == 9);

    rectangular(2, 3) = 5.0;
    assert(rectangular(2, 3) == 5.0);

    // const access check
    const Matrix &cr = rectangular;
    assert(cr(2, 3) == 5.0);

    // at() bounds check
    bool threw = false;
    try {
        rectangular.at(10, 10);
    } catch (const std::out_of_range &) {
        threw = true;
    }
    assert(threw);
}

// ==========================================
// Ownership & Copying
// ==========================================

void test_copy_constructor() {
    Matrix first(10, 10);
    first(5, 5) = 7.0;

    Matrix second = first;
    assert(second.get_row_count() == 10);
    assert(second.get_coloumn_count() == 10);
    assert(second(5, 5) == 7.0);

    // Modifying second must not affect first (deep copy)
    second(5, 5) = 88.0;
    assert(second(5, 5) == 88.0);
    assert(first(5, 5) == 7.0);
}

void test_copy_assignment() {
    Matrix a(2, 2);
    a(0, 0) = 1.0;
    Matrix b(3, 4);
    b = a; // Reshape & copy

    assert(b.get_row_count() == 2);
    assert(b.get_coloumn_count() == 2);
    assert(b(0, 0) == 1.0);

    // Self assignment check
    a = a;
    assert(a.get_row_count() == 2);
    assert(a(0, 0) == 1.0);
}

// ==========================================
// Move
// ==========================================

void test_move_constructor() {
    Matrix source(8, 20);
    source(1, 2) = 42.0;
    source(2, 3) = 99.5;

    Matrix target = std::move(source);

    assert(target.get_row_count() == 8);
    assert(target.get_coloumn_count() == 20);
    assert(target(1, 2) == 42.0);
    assert(target(2, 3) == 99.5);

    // Source must be left empty and destructible
    assert(source.get_row_count() == 0);
    assert(source.get_coloumn_count() == 0);
}

void test_move_assignment() {
    Matrix a(3, 3);
    a(1, 1) = 7.5;
    Matrix b(2, 2);

    b = std::move(a);

    assert(b.get_row_count() == 3);
    assert(b.get_coloumn_count() == 3);
    assert(b(1, 1) == 7.5);
    assert(a.get_row_count() == 0);
    assert(a.get_coloumn_count() == 0);

    // Self-move assignment safety: a = std::move(a)
    b = std::move(b);
    assert(b.get_row_count() == 3);
    assert(b(1, 1) == 7.5);
}

void test_growing_vector() {
    static_assert(std::is_nothrow_move_constructible_v<Matrix>,
                  "Matrix must be noexcept move-constructible for std::vector to move elements");

    std::vector<Matrix> vec;
    vec.emplace_back(2, 2);
    vec[0](0, 0) = 42.0;

    double *original_buffer = &vec[0](0, 0);

    size_t initial_cap = vec.capacity();
    while (vec.size() <= initial_cap) {
        vec.emplace_back(2, 2);
    }

    // Handed over via move constructor rather than deep-copied
    assert(&vec[0](0, 0) == original_buffer);
    assert(vec[0](0, 0) == 42.0);
}

// ==========================================
// Arithimetic Operators
// ==========================================

void test_initializer_list() {
    Matrix m{
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    };
    assert(m.get_row_count() == 2);
    assert(m.get_coloumn_count() == 3);
    assert(m(0, 0) == 1.0);
    assert(m(0, 2) == 3.0);
    assert(m(1, 1) == 5.0);
    assert(m(1, 2) == 6.0);

    // Unequal row lengths must throw
    bool threw = false;
    try {
        Matrix bad{
            {1.0, 2.0},
            {3.0, 4.0, 5.0}
        };
        (void)bad;
    } catch (const std::invalid_argument &) {
        threw = true;
    }
    assert(threw && "Ragged initializer_list must throw std::invalid_argument");
}

void test_addition_and_subtraction() {
    Matrix a{
        {1.0, 2.0},
        {3.0, 4.0}
    };
    Matrix b{
        {5.0, 6.0},
        {7.0, 8.0}
    };

    // Commutativity: A + B == B + A
    assert(approx_equal(a + b, b + a));

    // Inverse: (A + B) - B ≈ A
    assert(approx_equal((a + b) - b, a));

    // Compound assignment: += and -=
    Matrix c = a;
    c += b;
    assert(approx_equal(c, a + b));
    c -= b;
    assert(approx_equal(c, a));

    // Unary minus: -A + A ≈ 0
    Matrix zeros(2, 2);
    assert(approx_equal(-a + a, zeros));
}

void test_scalar_multiplication_and_division() {
    Matrix a{
        {1.0, 2.0},
        {3.0, 4.0}
    };

    // Scalar * Matrix == Matrix * Scalar == A + A
    assert(approx_equal(2.0 * a, a * 2.0));
    assert(approx_equal(2.0 * a, a + a));

    // Matrix *= scalar
    Matrix scaled = a;
    scaled *= 3.0;
    assert(approx_equal(scaled, 3.0 * a));

    // Matrix /= scalar
    scaled /= 3.0;
    assert(approx_equal(scaled, a));

    // Division by zero throws
    bool threw = false;
    try {
        scaled /= 0.0;
    } catch (const std::invalid_argument &) {
        threw = true;
    }
    assert(threw);
}

void test_transpose() {
    Matrix a{
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    };

    // Rectangular transpose
    Matrix at = a.transpose();
    assert(at.get_row_count() == 3);
    assert(at.get_coloumn_count() == 2);
    assert(at(0, 0) == 1.0);
    assert(at(0, 1) == 4.0);
    assert(at(2, 1) == 6.0);

    // Double transpose identity: (A^T)^T == A
    assert(approx_equal(a.transpose().transpose(), a));
    assert(approx_equal(transpose(transpose(a)), a));
}

void test_identity_matrix() {
    Matrix id = Matrix::identity(3);
    assert(id.get_row_count() == 3);
    assert(id.get_coloumn_count() == 3);
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i == j) {
                assert(id(i, j) == 1.0);
            } else {
                assert(id(i, j) == 0.0);
            }
        }
    }
}

void test_shape_mismatch() {
    Matrix a(2, 3);
    Matrix b(3, 2);

    bool threw_add = false;
    try {
        Matrix c = a + b;
        (void)c;
    } catch (const std::invalid_argument &) {
        threw_add = true;
    }
    assert(threw_add);

    bool threw_sub = false;
    try {
        Matrix d = a - b;
        (void)d;
    } catch (const std::invalid_argument &) {
        threw_sub = true;
    }
    assert(threw_sub);
}

void test_stream_output() {
    Matrix a{
        {1.0, 2.0},
        {3.0, 4.0}
    };
    std::ostringstream oss;
    oss << a;
    std::string output = oss.str();
    assert(!output.empty());
    assert(output.find("1") != std::string::npos);
    assert(output.find("4") != std::string::npos);
}

// ==========================================
// Multiplication
// ==========================================

void benchmark_fast_slow_multi() {
    std::cout << "Multiplication speed benchmark:\n";
    const size_t n = 200;
    const Matrix a = generate_matrix(1, n, n);
    const Matrix b = generate_matrix(2, n, n);

    auto start_slow = std::chrono::high_resolution_clock::now();
    Matrix slow_res = slow_multiply(a, b);
    auto end_slow = std::chrono::high_resolution_clock::now();

    auto start_fast = std::chrono::high_resolution_clock::now();
    Matrix fast_res = a * b;
    auto end_fast = std::chrono::high_resolution_clock::now();

    assert(approx_equal(slow_res, fast_res));

    auto slow_dur = std::chrono::duration_cast<std::chrono::microseconds>(end_slow - start_slow);
    auto fast_dur = std::chrono::duration_cast<std::chrono::microseconds>(end_fast - start_fast);


    std::cout << "  Matrix size              : " << n << "x" << n << "\n";
    std::cout << "  Slow multiply (i-j-k)    : " << slow_dur.count() << " micros\n";
    std::cout << "  Fast multiply (i-k-j)    : " << fast_dur.count() << " micros\n";
    double speedup = fast_dur.count() > 0 ? static_cast<double>(slow_dur.count()) / static_cast<double>(fast_dur.count()) : 0.0;

    std::cout << "  Speedup                  : " << std::fixed << std::setprecision(2) << speedup << "x\n";
}


void test_solver() {
    Matrix A{
        {1.0,  2.0,  1.0,  1.0},
        {2.0,  1.0, -1.0,  1.0},
        {1.0, -1.0,  2.0, -1.0},
        {1.0,  1.0,  1.0,  2.0}
    };

    Matrix b{
        { 7.0},
        { 8.0},
        {-6.0},
        { 8.0}
    };

    Matrix x = solve(A,b);
    std::cout << "x:\n" << x << "\n";
}

// ==========================================
// main
// ==========================================

int main() {
    std::cout << "Running Matrix test suite...\n";


    test_dimensions_and_access();


    test_copy_constructor();
    test_copy_assignment();


    test_move_constructor();
    test_move_assignment();
    test_growing_vector();


    test_initializer_list();
    test_addition_and_subtraction();
    test_scalar_multiplication_and_division();
    test_transpose();
    test_identity_matrix();
    test_shape_mismatch();
    test_stream_output();


    test_sum_speed();
    //benchmark_fast_slow_multi();
    test_solver();
    std::cout << "All tests passed successfully!\n";
    return 0;
}
