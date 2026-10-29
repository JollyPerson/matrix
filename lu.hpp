#pragma once

#include "matrix.hpp"
#include <cstddef>
#include <vector>
namespace matrix {
    class LUDecomposition {
    private:
        Matrix<double> factors;
        std::vector<std::size_t> permutation;
        std::size_t swap_count;

    public:
        explicit LUDecomposition(const Matrix<double> &A);
        Matrix<double> factor();

        Matrix<double> solve(const Matrix<double> &b) const;

        const Matrix<double> &get_factors() const;
        const std::vector<std::size_t> &get_permutation() const;
        std::size_t get_swap_count() const;

        double determinant() const;
        Matrix<double> inverse() const;
    };
}