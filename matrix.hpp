#pragma once

#include <cstddef>

class Matrix{
private:
    std::size_t rows;
    std::size_t cols;
    double* elements;

public:
    Matrix(std::size_t row, std::size_t col);

    std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;
};