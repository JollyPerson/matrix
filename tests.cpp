#include "tests/test_declarations.hpp"
#include "tests/test_helpers.hpp"

#include <iostream>

int main() {
    std::cout << "Running Matrix<double> test suite...\n";

    std::cout << "Running reusable float tests...\n";
    run_reusable_tests<float>();
    std::cout << "Running reusable double tests...\n";
    run_reusable_tests<double>();

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
    test_solver();
    test_solver_partial_pivoting();
    test_solver_tiny_pivot();
    test_solver_singular_matrix();
    test_stability_epsilon_pivoting();
    test_stability_random_matrices();
    test_stability_hilbert_ill_conditioning();
    test_stability_singular_vs_perturbed();
    test_lu_factor_reconstruction();
    test_lu_solver();
    test_lu_determinant();
    test_lu_inverse();
    test_lu_singular();
    benchmark_lu_vs_gaussian_multisolve();

    std::cout << "All tests passed successfully!\n";
    return 0;
}
