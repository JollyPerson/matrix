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
    assert(residual(A, b, x) < 1e-12);
}

void test_solver_partial_pivoting() {
    Matrix A{
        {0.0, 1.0},
        {1.0, 1.0}
    };
    Matrix b{
        {1.0},
        {2.0}
    };

    Matrix x = solve(A, b);

    assert(std::abs(x(0, 0) - 1.0) < 1e-12);
    assert(std::abs(x(1, 0) - 1.0) < 1e-12);
    assert(residual(A, b, x) < 1e-12);
}

void test_solver_tiny_pivot() {
    constexpr double epsilon = 1e-20;
    Matrix A{
        {epsilon, 1.0},
        {1.0, 1.0}
    };
    Matrix b{
        {1.0},
        {2.0}
    };

    Matrix x = solve(A, b);

    assert(std::abs(x(0, 0) - 1.0) < 1e-12);
    assert(std::abs(x(1, 0) - 1.0) < 1e-12);
    assert(residual(A, b, x) < 1e-12);
}

void test_solver_singular_matrix() {
    Matrix A{
        {1.0, 2.0},
        {2.0, 4.0}
    };
    Matrix b{
        {3.0},
        {6.0}
    };

    bool threw = false;
    try {
        (void)solve(A, b);
    } catch (const std::runtime_error &) {
        threw = true;
    }

    assert(threw);
}

// ==========================================
// Numerical Stability Experiments
// ==========================================

// Forward elimination without pivoting to demonstrate catastrophic rounding
Matrix solve_naive_no_pivot(Matrix A, Matrix b) {
    const size_t n = A.get_row_count();
    for (size_t i = 0; i < n; ++i) {
        const double pivot = A(i, i);
        if (pivot == 0.0) {
            throw std::runtime_error("Zero pivot encountered in naive elimination");
        }
        for (size_t j = i + 1; j < n; ++j) {
            const double factor = A(j, i) / pivot;
            A(j, i) = 0.0;
            for (size_t k = i + 1; k < n; ++k) {
                A(j, k) -= factor * A(i, k);
            }
            b(j, 0) -= factor * b(i, 0);
        }
    }
    Matrix x(n, 1);
    for (size_t step = 0; step < n; ++step) {
        size_t i = (n - 1) - step;
        double sum = b(i, 0);
        for (size_t j = i + 1; j < n; ++j) {
            sum -= A(i, j) * x(j, 0);
        }
        x(i, 0) = sum / A(i, i);
    }
    return x;
}

Matrix make_hilbert(size_t n) {
    Matrix H(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            H(i, j) = 1.0 / static_cast<double>(i + j + 1);
        }
    }
    return H;
}

double max_forward_error(const Matrix &computed_x, const Matrix &true_x) {
    double err = 0.0;
    for (size_t i = 0; i < computed_x.get_row_count(); ++i) {
        err = std::max(err, std::abs(computed_x(i, 0) - true_x(i, 0)));
    }
    return err;
}

void run_stability_experiments() {
    std::cout << "\n==========================================\n";
    std::cout << "Running Numerical Stability Experiments\n";
    std::cout << "==========================================\n\n";

    // ----------------------------------------------------
    // Experiment 1: Epsilon system with and without pivoting
    // ----------------------------------------------------
    std::cout << "--- Experiment 1: Tiny Pivot (epsilon = 1e-20) ---\n";
    constexpr double eps = 1e-20;
    Matrix A_eps{{eps, 1.0}, {1.0, 1.0}};
    Matrix b_eps{{1.0}, {2.0}};

    Matrix x_no_pivot = solve_naive_no_pivot(A_eps, b_eps);
    Matrix x_with_pivot = solve(A_eps, b_eps);

    std::cout << "Without pivoting : x = [" << x_no_pivot(0, 0) << ", " << x_no_pivot(1, 0) << "]\n";
    std::cout << "With pivoting    : x = [" << x_with_pivot(0, 0) << ", " << x_with_pivot(1, 0) << "]\n";
    std::cout << "True solution    : x ≈ [1.0, 1.0]\n\n";

    // ----------------------------------------------------
    // Experiment 2: Random matrices residual check
    // ----------------------------------------------------
    std::cout << "--- Experiment 2: Random Matrices (Residual) ---\n";
    std::cout << std::left << std::setw(10) << "Size (N)"
              << std::setw(25) << "Residual (||b - Ax||_inf)" << "\n";
    for (size_t n : {10, 100, 500}) {
        Matrix A = generate_matrix(100 + static_cast<int>(n), n, n, -100.0, 100.0);
        Matrix b = generate_matrix(200 + static_cast<int>(n), n, 1, -100.0, 100.0);
        Matrix x = solve(A, b);
        double res = residual(A, b, x);
        std::cout << std::left << std::setw(10) << n
                  << std::setw(25) << std::scientific << std::setprecision(6) << res << "\n";
    }
    std::cout << "\n";

    // ----------------------------------------------------
    // Experiment 3: Hilbert Matrices (Residual vs Forward Error)
    // ----------------------------------------------------
    std::cout << "--- Experiment 3: Hilbert Matrix Conditioning ---\n";
    std::cout << std::left << std::setw(10) << "Size (N)"
              << std::setw(25) << "Residual (||b - Hx||)"
              << std::setw(25) << "Forward Error (||x - x_true||)" << "\n";

    for (size_t n : {5, 8, 10, 12, 14}) {
        Matrix H = make_hilbert(n);
        Matrix x_true(n, 1);
        for (size_t i = 0; i < n; ++i) {
            x_true(i, 0) = 1.0;
        }
        Matrix b = H * x_true;

        try {
            Matrix x_comp = solve(H, b);
            double res = residual(H, b, x_comp);
            double f_err = max_forward_error(x_comp, x_true);

            std::cout << std::left << std::setw(10) << n
                      << std::setw(25) << std::scientific << std::setprecision(6) << res
                      << std::setw(25) << std::scientific << std::setprecision(6) << f_err << "\n";
        } catch (const std::runtime_error &e) {
            std::cout << std::left << std::setw(10) << n
                      << "[Rejected by solver: " << e.what() << "]\n";
        }
    }
    std::cout << "\n";

    // ----------------------------------------------------
    // Experiment 4: Singular vs Near-Singular Matrix
    // ----------------------------------------------------
    std::cout << "--- Experiment 4: Singular vs Near-Singular Matrix ---\n";
    Matrix A_sing{{1.0, 2.0}, {1.0, 2.0}};
    Matrix b_sing{{3.0}, {3.0}};
    bool threw_sing = false;
    try {
        (void)solve(A_sing, b_sing);
    } catch (const std::runtime_error &) {
        threw_sing = true;
    }
    std::cout << "Identical rows (exact singular): Threw expected exception = "
              << (threw_sing ? "YES" : "NO") << "\n";

    Matrix A_near{{1.0, 2.0}, {1.0, 2.0 + 1e-13}};
    try {
        Matrix x_near = solve(A_near, b_sing);
        std::cout << "Perturbed by 1e-13: Solved x = [" << x_near(0, 0) << ", " << x_near(1, 0)
                  << "], Residual = " << residual(A_near, b_sing, x_near) << "\n";
    } catch (const std::exception &e) {
        std::cout << "Perturbed by 1e-13: Threw exception: " << e.what() << "\n";
    }
    std::cout << "==========================================\n\n";
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
    test_solver_partial_pivoting();
    test_solver_tiny_pivot();
    test_solver_singular_matrix();
    run_stability_experiments();
    std::cout << "All tests passed successfully!\n";
    return 0;
}
