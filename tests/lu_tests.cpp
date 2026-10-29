#include "test_helpers.hpp"

void test_lu_factor_reconstruction() {
    Matrix<double> A{
        {1.0,  2.0,  1.0,  1.0},
        {2.0,  1.0, -1.0,  1.0},
        {1.0, -1.0,  2.0, -1.0},
        {1.0,  1.0,  1.0,  2.0}
    };

    matrix::LUDecomposition lu(A);
    const size_t n = A.get_row_count();

    Matrix<double> L = Matrix<double>::identity(n);
    Matrix<double> U(n, n);
    const Matrix<double> &F = lu.get_factors();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i > j) {
                L(i, j) = F(i, j);
            } else {
                U(i, j) = F(i, j);
            }
        }
    }

    Matrix<double> PA(n, n);
    const auto &P = lu.get_permutation();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            PA(i, j) = A(P[i], j);
        }
    }

    assert(approx_equal(L * U, PA, 1e-12));
}

void test_lu_solver() {
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

    matrix::LUDecomposition lu(A);
    Matrix<double> x = lu.solve(b);

    assert(residual(A, b, x) < 1e-12);
}

void test_lu_determinant() {
    Matrix<double> A{
        {2.0, 1.0},
        {1.0, 3.0}
    };
    matrix::LUDecomposition lu_A(A);
    assert(std::abs(lu_A.determinant() - 5.0) < 1e-12);

    Matrix<double> B{
        {0.0, 1.0},
        {1.0, 0.0}
    };
    matrix::LUDecomposition lu_B(B);
    assert(std::abs(lu_B.determinant() - (-1.0)) < 1e-12);

    // det(AB) == det(A) * det(B)
    Matrix<double> AB = A * B;
    matrix::LUDecomposition lu_AB(AB);
    assert(std::abs(lu_AB.determinant() - (lu_A.determinant() * lu_B.determinant())) < 1e-11);
}

void test_lu_inverse() {
    Matrix<double> A{
        {1.0,  2.0,  1.0,  1.0},
        {2.0,  1.0, -1.0,  1.0},
        {1.0, -1.0,  2.0, -1.0},
        {1.0,  1.0,  1.0,  2.0}
    };

    matrix::LUDecomposition lu(A);
    Matrix<double> A_inv = lu.inverse();

    assert(approx_equal(A * A_inv, Matrix<double>::identity(A.get_row_count()), 1e-12));
    assert(approx_equal(A_inv * A, Matrix<double>::identity(A.get_row_count()), 1e-12));
}

void test_lu_singular() {
    Matrix<double> singular{
        {1.0, 2.0},
        {2.0, 4.0}
    };

    bool threw = false;
    try {
        matrix::LUDecomposition lu(singular);
        (void)lu;
    } catch (const std::runtime_error &) {
        threw = true;
    }

    assert(threw);
}

void benchmark_lu_vs_gaussian_multisolve() {
    std::cout << "\n--- Benchmark: 10 Solves (Full Solve vs LU Re-use) ---\n";
    const size_t n = 200;
    Matrix<double> A = generate_matrix(777, n, n, -50.0, 50.0);
    std::vector<Matrix<double>> rhs_list;
    rhs_list.reserve(10);
    for (int i = 0; i < 10; ++i) {
        rhs_list.push_back(generate_matrix(888 + i, n, 1, -50.0, 50.0));
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    for (const auto &b : rhs_list) {
        Matrix<double> x = solve(A, b);
        (void)x;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    auto full_solve_dur = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    auto t2 = std::chrono::high_resolution_clock::now();
    matrix::LUDecomposition lu(A);
    for (const auto &b : rhs_list) {
        Matrix<double> x = lu.solve(b);
        (void)x;
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    auto lu_dur = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);

    std::cout << "  10x Full solve (repeats cubic elimination) : " << full_solve_dur.count() << " micros\n";
    std::cout << "  1x Factor + 10x LU solves                  : " << lu_dur.count() << " micros\n";
    double speedup = lu_dur.count() > 0 ? static_cast<double>(full_solve_dur.count()) / static_cast<double>(lu_dur.count()) : 0.0;
    std::cout << "  Speedup                                    : " << std::fixed << std::setprecision(2) << speedup << "x\n\n";
}

// ==========================================
// main
// ==========================================

