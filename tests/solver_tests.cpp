#include "test_helpers.hpp"

void test_solver() {
    Matrix<double> A{
        {1.0,  2.0,  1.0,  1.0},
        {2.0,  1.0, -1.0,  1.0},
        {1.0, -1.0,  2.0, -1.0},
        {1.0,  1.0,  1.0,  2.0}
    };

    Matrix<double> b{
        { 7.0},
        { 8.0},
        {-6.0},
        { 8.0}
    };

    Matrix<double> x = solve(A,b);
    std::cout << "x:\n" << x << "\n";
    assert(residual(A, b, x) < 1e-12);
}

void test_solver_partial_pivoting() {
    Matrix<double> A{
        {0.0, 1.0},
        {1.0, 1.0}
    };
    Matrix<double> b{
        {1.0},
        {2.0}
    };

    Matrix<double> x = solve(A, b);

    assert(std::abs(x(0, 0) - 1.0) < 1e-12);
    assert(std::abs(x(1, 0) - 1.0) < 1e-12);
    assert(residual(A, b, x) < 1e-12);
}

void test_solver_tiny_pivot() {
    constexpr double epsilon = 1e-20;
    Matrix<double> A{
        {epsilon, 1.0},
        {1.0, 1.0}
    };
    Matrix<double> b{
        {1.0},
        {2.0}
    };

    Matrix<double> x = solve(A, b);

    assert(std::abs(x(0, 0) - 1.0) < 1e-12);
    assert(std::abs(x(1, 0) - 1.0) < 1e-12);
    assert(residual(A, b, x) < 1e-12);
}

void test_solver_singular_matrix() {
    Matrix<double> A{
        {1.0, 2.0},
        {2.0, 4.0}
    };
    Matrix<double> b{
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
Matrix<double> solve_naive_no_pivot(Matrix<double> A, Matrix<double> b) {
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
    Matrix<double> x(n, 1);
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

Matrix<double> make_hilbert(size_t n) {
    Matrix<double> H(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            H(i, j) = 1.0 / static_cast<double>(i + j + 1);
        }
    }
    return H;
}

double max_forward_error(const Matrix<double> &computed_x, const Matrix<double> &true_x) {
    double err = 0.0;
    for (size_t i = 0; i < computed_x.get_row_count(); ++i) {
        err = std::max(err, std::abs(computed_x(i, 0) - true_x(i, 0)));
    }
    return err;
}

void test_stability_epsilon_pivoting() {
    std::cout << "\n--- Stability Test: Tiny Pivot (epsilon = 1e-20) ---\n";
    constexpr double eps = 1e-20;
    Matrix<double> A_eps{{eps, 1.0}, {1.0, 1.0}};
    Matrix<double> b_eps{{1.0}, {2.0}};

    Matrix<double> x_no_pivot = solve_naive_no_pivot(A_eps, b_eps);
    Matrix<double> x_with_pivot = solve(A_eps, b_eps);

    std::cout << "Without pivoting : x = [" << x_no_pivot(0, 0) << ", " << x_no_pivot(1, 0) << "]\n";
    std::cout << "With pivoting    : x = [" << x_with_pivot(0, 0) << ", " << x_with_pivot(1, 0) << "]\n";

    assert(std::abs(x_with_pivot(0, 0) - 1.0) < 1e-12);
    assert(std::abs(x_with_pivot(1, 0) - 1.0) < 1e-12);
    assert(std::abs(x_no_pivot(0, 0) - 0.0) < 1e-12);
}

void test_stability_random_matrices() {
    std::cout << "\n--- Stability Test: Random Matrices Residuals ---\n";
    std::cout << std::left << std::setw(10) << "Size (N)"
              << std::setw(25) << "Residual (||b - Ax||_inf)" << "\n";
    for (size_t n : {10, 100, 500}) {
        Matrix<double> A = generate_matrix(100 + static_cast<int>(n), n, n, -100.0, 100.0);
        Matrix<double> b = generate_matrix(200 + static_cast<int>(n), n, 1, -100.0, 100.0);
        Matrix<double> x = solve(A, b);
        double res = residual(A, b, x);
        std::cout << std::left << std::setw(10) << n
                  << std::setw(25) << std::scientific << std::setprecision(6) << res << "\n";
        assert(res < 1e-8);
    }
}

void test_stability_hilbert_ill_conditioning() {
    std::cout << "\n--- Stability Test: Hilbert Matrix<double> Conditioning ---\n";
    std::cout << std::left << std::setw(10) << "Size (N)"
              << std::setw(25) << "Residual (||b - Hx||)"
              << std::setw(25) << "Forward Error (||x - x_true||)" << "\n";

    for (size_t n : {5, 8, 10, 12, 14}) {
        Matrix<double> H = make_hilbert(n);
        Matrix<double> x_true(n, 1);
        for (size_t i = 0; i < n; ++i) {
            x_true(i, 0) = 1.0;
        }
        Matrix<double> b = H * x_true;

        try {
            Matrix<double> x_comp = solve(H, b);
            double res = residual(H, b, x_comp);
            double f_err = max_forward_error(x_comp, x_true);

            std::cout << std::left << std::setw(10) << n
                      << std::setw(25) << std::scientific << std::setprecision(6) << res
                      << std::setw(25) << std::scientific << std::setprecision(6) << f_err << "\n";

            assert(res < 1e-12);
        } catch (const std::runtime_error &e) {
            std::cout << std::left << std::setw(10) << n
                      << "[Rejected by solver: " << e.what() << "]\n";
            assert(n >= 12);
        }
    }
}

void test_stability_singular_vs_perturbed() {
    std::cout << "\n--- Stability Test: Singular vs Near-Singular Matrix<double> ---\n";
    Matrix<double> A_sing{{1.0, 2.0}, {1.0, 2.0}};
    Matrix<double> b_sing{{3.0}, {3.0}};
    bool threw_sing = false;
    try {
        (void)solve(A_sing, b_sing);
    } catch (const std::runtime_error &) {
        threw_sing = true;
    }
    assert(threw_sing);

    Matrix<double> A_near{{1.0, 2.0}, {1.0, 2.0 + 1e-13}};
    try {
        Matrix<double> x_near = solve(A_near, b_sing);
        std::cout << "Perturbed by 1e-13: Solved x = [" << x_near(0, 0) << ", " << x_near(1, 0)
                  << "], Residual = " << residual(A_near, b_sing, x_near) << "\n";
    } catch (const std::exception &e) {
        std::cout << "Perturbed by 1e-13: Threw exception: " << e.what() << "\n";
    }
}

// ==========================================
// LU Decomposition Tests
// ==========================================

