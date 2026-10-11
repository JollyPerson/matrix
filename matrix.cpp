#include "matrix.hpp"
#include <stdexcept>
#include <assert.h>

Matrix::Matrix(std::size_t row, std::size_t coloumn)
    : rows(row), cols(coloumn), elements(nullptr)
{
    elements = new double[rows*cols];
}

Matrix::~Matrix(){
    delete[] elements;
}


//Will break if rows*cols is > size_t max i.e (size_t_max / rows) < cols
Matrix::Matrix(Matrix &old): rows(old.rows), cols(old.cols){
    size_t size = rows*cols;
    elements = new double[size];
    for(size_t i = 0; i < size; i++){
        elements[i] = old.elements[i];
    }
}

Matrix& Matrix::operator=(const Matrix& old){
    //Check if it equals itself
    if(this == &old){
        return *this;
    }

    delete[] this->elements;
    this->rows = old.rows;
    this->cols = old.cols;
    size_t size = rows*cols;
    this->elements = new double[size];
    for(size_t i=0; i<size; i++){
        this->elements[i] = old.elements[i];
    }

    return *this;

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

//RW with assertions for debugging, will inline when production mode
double& Matrix::operator()(size_t i, size_t j){
    assert(i < rows && "Row index out of bounds");
    assert(j < cols && "Coloumn index of out of bounds");
    return elements[offset(i,j)];
}

//R will inline for const Matrix&
double Matrix::operator()(size_t i, size_t j) const{
    assert(i < rows && "Row index out of bounds");
    assert(j < cols && "Coloumn index of out of bounds");
    return elements[offset(i,j)];
}


//will check at runtime if in bounds
double& Matrix::at(std::size_t i, std::size_t j) {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i,j)];
}


//Will check at runtime if in bounds
double Matrix::at(std::size_t i, std::size_t j) const {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("matrix indicies out of bounds");
    }
    return elements[offset(i,j)];
}

void Matrix::flush_zero(){

}

void Matrix::swap(Matrix &other) noexcept{
    std::swap(this->elements,other.elements);
    std::swap(this->cols,other.cols);
    std::swap(this->rows,other.rows);
}