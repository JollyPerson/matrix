#pragma once

#include <cstddef>

class Matrix{
private:
    std::size_t rows;
    std::size_t cols;
    double* elements;

    std::size_t offset(size_t x, size_t y) const;

public:
    Matrix(std::size_t row, std::size_t col);

    std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;
};