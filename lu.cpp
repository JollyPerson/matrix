#include <lu.hpp>

namespace matrix {
LUDecomposition::LUDecomposition(const Matrix &A) {
    // Implementation for LUDecomposition constructor
}

Matrix LUDecomposition::solve(const Matrix &b) const {
    size_t rowB = b.get_row_count();
    size_t colB = b.get_coloumn_count();

    if (rowB != factors.get_row_count() || colB != 1) {
        throw std::invalid_argument("Matrix B dimensions do not match for solving.");
    }

const Matrix &LUDecomposition::get_factors() const {
    return factors;
}

const std::vector<size_t> &LUDecomposition::get_permutation() const {
    return permutation;
}

size_t LUDecomposition::get_swap_count() const {
    return swap_count;
}

explicit LUDecompositon::LUDecomposition(const Matrix &A){
    // Implementation for LUDecomposition constructor
}

double LUDecomposition::determinat() const {
    // Implementation for calculating the determinant
}

Matrix LUDecomposition::inverse() const {
    // Implementation for calculating the inverse
}
}