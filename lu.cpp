#include "lu.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <numeric>

namespace matrix
{
    LUDecomposition::LUDecomposition(const Matrix &A) : factors(A), permutation(A.get_row_count()), swap_count(0)
    {
        const std::size_t n = A.get_row_count();
        if (n == 0)
        {
            throw std::invalid_argument(
                "LU decomposition requires a non-empty matrix");
        }
        if (A.get_coloumn_count() != n)
        {
            throw std::invalid_argument(
                "LU decomposition requires a square matrix");
        }
        std::iota(permutation.begin(), permutation.end(), 0);
    }

    Matrix LUDecomposition::factor()
    {
        const size_t n = factors.get_row_count();
        const double scale = maxValue(factors);
        const double tolerance = std::numeric_limits<double>::epsilon() * scale * static_cast<double>(n);

        for (size_t k = 0; k < n; ++k)
        {
            size_t pivot_row = k;
            double largest = abs(factors(k, k));
            for (size_t row = k + 1; row < n; ++row)
            {
                const double candidate = abs(factors(row, k));
                if (candidate > largest)
                {
                    largest = candidate;
                    pivot_row = row;
                }
                if (largest < tolerance)
                {
                    throw std::runtime_error("Matrix is singular to working precision.");
                }
                if (pivot_row != k)
                {
                    std::swap(permutation[k], permutation[pivot_row]);
                    factors.swap_rows(k, pivot_row);
                    ++swap_count;
                }
                for (std::size_t row = k + 1; row < n; ++row)
                {
                    const double multiplier =
                        factors(row, k) / factors(k, k);

                    factors(row, k) = multiplier;

                    for (std::size_t column = k + 1;
                         column < n;
                         ++column)
                    {
                        factors(row, column) -=
                            multiplier * factors(k, column);
                    }
                }
                // find pivot
                // check pivot
                // swap
                // store multipliers update U
            }
        }
    }

    Matrix LUDecomposition::solve(const Matrix &b) const
    {
        size_t rowB = b.get_row_count();
        size_t colB = b.get_coloumn_count();

        if (rowB != factors.get_row_count() || colB != 1)
        {
            throw std::invalid_argument("Matrix B dimensions do not match for solving.");
        }
    }
    const Matrix &LUDecomposition::get_factors() const
    {
        return factors;
    }

    const std::vector<size_t> &LUDecomposition::get_permutation() const
    {
        return permutation;
    }

    size_t LUDecomposition::get_swap_count() const
    {
        return swap_count;
    }

    double LUDecomposition::determinant() const
    {
        // Implementation for calculating the determinant
    }

    Matrix LUDecomposition::inverse() const
    {
        // Implementation for calculating the inverse
    }
}
