#pragma once

#include "matrix.hpp"
#include <cstddef>
#include <concepts>
#include <vector>

namespace matrix {
    template <std::floating_point T>
    class LUDecomposition {
    private:
        Matrix<T> factors;
        std::vector<std::size_t> permutation;
        std::size_t swap_count;

    public:
        explicit LUDecomposition(const Matrix<T>& A);
        Matrix<T> factor();

        Matrix<T> solve(const Matrix<T>& b) const;

        const Matrix<T>& get_factors() const;
        const std::vector<std::size_t>& get_permutation() const;
        std::size_t get_swap_count() const;

        T determinant() const;
        Matrix<T> inverse() const;
    };
}

#include "lu.tpp"