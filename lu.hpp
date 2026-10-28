#pragma once

#include "matrix.hpp"
#include <cstddef>
#include <vector>
namespace matrix {
    class LUDecomposition {
    private:
        Matrix factors;
        std::vector<std::size_t> permutation;
        std::size_t swap_count;

    public:
        explicit LUDecomposition(const Matrix &A);
        Matrix factor();

        Matrix solve(const Matrix &b) const;

        const Matrix &get_factors() const;
        const std::vector<std::size_t> &get_permutation() const;
        std::size_t get_swap_count() const;

        double determinant() const;
        Matrix inverse() const;
    };
}