#include "test_helpers.hpp"

void test_dimensions_and_access() {
    Matrix<double> empty(0, 0);
    assert(empty.get_row_count() == 0);
    assert(empty.get_coloumn_count() == 0);

    Matrix<double> square(2, 2);
    assert(square.get_row_count() == 2);
    assert(square.get_coloumn_count() == 2);
    assert(square(0, 0) == 0.0); // Verifies zero-initialization

    Matrix<double> rectangular(5, 9);
    assert(rectangular.get_row_count() == 5);
    assert(rectangular.get_coloumn_count() == 9);

    rectangular(2, 3) = 5.0;
    assert(rectangular(2, 3) == 5.0);

    // const access check
    const Matrix<double> &cr = rectangular;
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
    Matrix<double> first(10, 10);
    first(5, 5) = 7.0;

    Matrix<double> second = first;
    assert(second.get_row_count() == 10);
    assert(second.get_coloumn_count() == 10);
    assert(second(5, 5) == 7.0);

    // Modifying second must not affect first (deep copy)
    second(5, 5) = 88.0;
    assert(second(5, 5) == 88.0);
    assert(first(5, 5) == 7.0);
}

void test_copy_assignment() {
    Matrix<double> a(2, 2);
    a(0, 0) = 1.0;
    Matrix<double> b(3, 4);
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
    Matrix<double> source(8, 20);
    source(1, 2) = 42.0;
    source(2, 3) = 99.5;

    Matrix<double> target = std::move(source);

    assert(target.get_row_count() == 8);
    assert(target.get_coloumn_count() == 20);
    assert(target(1, 2) == 42.0);
    assert(target(2, 3) == 99.5);

    // std::vector is empty after the move; the scalar dimensions are unchanged.
    assert(source.get_row_count() == 8);
    assert(source.get_coloumn_count() == 20);
}

void test_move_assignment() {
    Matrix<double> a(3, 3);
    a(1, 1) = 7.5;
    Matrix<double> b(2, 2);

    b = std::move(a);

    assert(b.get_row_count() == 3);
    assert(b.get_coloumn_count() == 3);
    assert(b(1, 1) == 7.5);
    assert(a.get_row_count() == 3);
    assert(a.get_coloumn_count() == 3);

}

void test_growing_vector() {
    static_assert(std::is_nothrow_move_constructible_v<Matrix<double>>,
                  "Matrix<double> must be noexcept move-constructible for std::vector to move elements");

    std::vector<Matrix<double>> vec;
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
