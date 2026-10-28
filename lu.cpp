#include "lu.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <numeric>

namespace matrix {
    LUDecomposition::LUDecomposition(const Matrix &A) : factors(A), permutation(A.get_row_count()), swap_count(0) {
        const std::size_t n = A.get_row_count();
        if (n == 0) {
            throw std::invalid_argument(
                "LU decomposition requires a non-empty matrix");
        }
        if (A.get_coloumn_count() != n) {
            throw std::invalid_argument(
                "LU decomposition requires a square matrix");
        }
        std::iota(permutation.begin(), permutation.end(), 0);
        factor();
    }

    Matrix LUDecomposition::factor() {
        const size_t n = factors.get_row_count();
        const double scale = maxValue(factors);
        const double tolerance = std::numeric_limits<double>::epsilon() * scale * static_cast<double>(n);

        for (size_t k = 0; k < n; ++k) {
            size_t pivot_row = k;
            double largest = std::abs(factors(k, k));
            for (size_t row = k + 1; row < n; ++row) {
                const double candidate = std::abs(factors(row, k));
                if (candidate > largest) {
                    largest = candidate;
                    pivot_row = row;
                }
            }
            if (largest < tolerance) {
                throw std::runtime_error("Matrix is singular to working precision.");
            }
            if (pivot_row != k) {
                std::swap(permutation[k], permutation[pivot_row]);
                factors.swap_rows(k, pivot_row);
                ++swap_count;
            }
            for (std::size_t row = k + 1; row < n; ++row) {
                const double multiplier =
                        factors(row, k) / factors(k, k);

                factors(row, k) = multiplier;

                for (std::size_t column = k + 1;
                     column < n;
                     ++column) {
                    factors(row, column) -=
                            multiplier * factors(k, column);
                }
            }
            // find pivot
            // check pivot
            // swap
            // store multipliers update U
        }
        return factors;
    }

    Matrix LUDecomposition::solve(const Matrix &b) const {
        size_t rowB = b.get_row_count();
        size_t colB = b.get_coloumn_count();
        size_t fr =  factors.get_row_count();

        if (rowB != factors.get_row_count() || colB != 1) {
            throw std::invalid_argument("Matrix B dimensions do not match for solving.");
        }

        Matrix y(fr,1);
        for (size_t i=0; i<fr; ++i) {
            y(i,0) = b(permutation[i], 0);
        }

        for (size_t i = 0; i<fr;++i) {
            double sum = y(i,0);
            for (size_t j = 0; j<i; ++j) {
                sum-= factors(i,j)*y(j,0);
            }
            y(i,0) = sum;
        }

        Matrix x(fr,1);
        for (size_t step = 0; step < fr; ++step) {
            size_t i = (fr - 1) - step;
            double sum = y(i,0);
            for (size_t j = i + 1; j < fr; ++j) {
                sum -= factors(i, j) * x(j, 0);
            }
            x(i, 0) = sum / factors(i, i);
        }
        return x;
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

    double LUDecomposition::determinant() const {
        const size_t fr = factors.get_row_count();
        //Parity of swaps forces sign of determinant
         double det = (swap_count %2==0) ? 1.0 : -1.0;
         for (size_t i = 0; i < fr; ++i) {
             det *= factors(i, i);
         }
         return det;
    }

    Matrix LUDecomposition::inverse() const {
        const size_t fr = factors.get_row_count();
        Matrix inv(fr,fr);
        for (size_t col = 0; col<fr;++col) {
            Matrix e(fr,1);
            e(col,0) = 1.0;
            Matrix x_col = solve(e);
            for (size_t row = 0; row<fr;++row) {
                inv(row,col) = x_col(row,0);
            }
        }
        return inv;
    }
}
