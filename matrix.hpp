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

    ~Matrix();

    Matrix(Matrix &old);

    Matrix& operator=(const Matrix& old);
   

    double& operator()(size_t i, size_t j);

    double operator()(size_t i, size_t j) const;

    double& at(size_t i, size_t j);

    double at(size_t i, size_t j) const;
    
    void flush_zero();

    void swap(Matrix &other) noexcept;
    

     std::size_t get_row_count() const;

    std::size_t get_coloumn_count() const;
};

void swap(Matrix &lhs,Matrix &rhs) noexcept;