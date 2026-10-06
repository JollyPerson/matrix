#include "matrix.hpp"

Matrix::Matrix(std::size_t row, std::size_t coloumn)
    : rows(row), cols(coloumn), elements(nullptr)
{

}

std::size_t Matrix::get_row_count() const{
    return rows;
}

std::size_t Matrix::get_coloumn_count() const{
    return cols;
}

std::size_t Matrix::offset(size_t x, size_t y) const {
    //Row Major offset
    return x * cols + y;
}