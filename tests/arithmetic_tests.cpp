#include "test_helpers.hpp"

void test_initializer_list() {
    Matrix<double> m{
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
        Matrix<double> bad{
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
    Matrix<double> a{
        {1.0, 2.0},
        {3.0, 4.0}
    };
    Matrix<double> b{
        {5.0, 6.0},
        {7.0, 8.0}
    };

    // Commutativity: A + B == B + A
    assert(approx_equal(a + b, b + a));

    // Inverse: (A + B) - B ≈ A
    assert(approx_equal((a + b) - b, a));

    // Compound assignment: += and -=
    Matrix<double> c = a;
    c += b;
    assert(approx_equal(c, a + b));
    c -= b;
    assert(approx_equal(c, a));

    // Unary minus: -A + A ≈ 0
    Matrix<double> zeros(2, 2);
    assert(approx_equal(-a + a, zeros));
}

void test_scalar_multiplication_and_division() {
    Matrix<double> a{
        {1.0, 2.0},
        {3.0, 4.0}
    };

    // Scalar * Matrix<double> == Matrix<double> * Scalar == A + A
    assert(approx_equal(2.0 * a, a * 2.0));
    assert(approx_equal(2.0 * a, a + a));

    // Matrix<double> *= scalar
    Matrix<double> scaled = a;
    scaled *= 3.0;
    assert(approx_equal(scaled, 3.0 * a));

    // Matrix<double> /= scalar
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
    Matrix<double> a{
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    };

    // Rectangular transpose
    Matrix<double> at = a.transpose();
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
    Matrix<double> id = Matrix<double>::identity(3);
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
    Matrix<double> a(2, 3);
    Matrix<double> b(3, 2);

    bool threw_add = false;
    try {
        Matrix<double> c = a + b;
        (void)c;
    } catch (const std::invalid_argument &) {
        threw_add = true;
    }
    assert(threw_add);

    bool threw_sub = false;
    try {
        Matrix<double> d = a - b;
        (void)d;
    } catch (const std::invalid_argument &) {
        threw_sub = true;
    }
    assert(threw_sub);
}

void test_stream_output() {
    Matrix<double> a{
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

