#include <lu.hpp>

namespace matrix {
LUDecomposition::LUDecomposition(const Matrix &A) {
    // Implementation for LUDecomposition constructor
}

Matrix LUDecomposition::solve(const Matrix &b) const {
    // Implementation for solving the system
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
} // namespace matrix