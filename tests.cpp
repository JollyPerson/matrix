#include <assert.h>
#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>

#include "matrix.hpp"


long double sum_matrix_row(const Matrix &matrix){
    long double sum = 0;
    size_t rows = matrix.get_row_count();
    size_t cols = matrix.get_coloumn_count();
    for(size_t x = 0; x < rows;x++){
        for(size_t y = 0; y<cols;y++){
            sum += matrix(x,y);
        }
    }
    return sum;
}

long double sum_matrix_coloumn(const Matrix &matrix){
    long double sum = 0;
    size_t rows = matrix.get_row_count();
    size_t cols = matrix.get_coloumn_count();
    for(size_t y = 0; y< cols; y++){
        for(size_t x = 0; x<rows; x++){
            sum += matrix(x,y);
        }
    }
    return sum;
}

Matrix generate_matrix(int seed, size_t rows, size_t cols, double min=0, double max=1000){
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> distr(min,max);
    Matrix matrix = Matrix(rows,cols);
    for(size_t x = 0; x < rows;x++){
        for(size_t y = 0; y<cols;y++){
            matrix(x,y) = distr(gen);
        }
    }
    return matrix;

}

void test_copy(){
    Matrix first = Matrix(10,10);
    first(5,5) = 7.0;
    assert(first(5,5) == 7.0);
    Matrix second = first;
    assert(first(5,5) == 7.0);
    assert(second(5,5) == 7.0);
    second(5,5) = 88.0;
    assert(second(5,5) == 88.0);
    assert(first(5,5) == 7.0);

}

void test_sum_speed(){
    auto start = std::chrono::high_resolution_clock::now();
    const Matrix a = generate_matrix(42, 4000,4000);
    auto rss = std::chrono::high_resolution_clock::now();
    long double rs = sum_matrix_coloumn(a);
    auto rsx = std::chrono::high_resolution_clock::now();
    auto css = std::chrono::high_resolution_clock::now();
    long double cs = sum_matrix_row(a);
    auto csx = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto rd = std::chrono::duration_cast<std::chrono::microseconds>(rsx-rss);
    auto cd = std::chrono::duration_cast<std::chrono::microseconds>(csx-css);
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Total  " << duration.count() << " micros" << std::endl;
    std::cout << "Totalr " << rd.count() << " micros" << std::endl;
    std::cout << "Totalc " << cd.count() << " micros" << std::endl;
    std::cout << std::setprecision(20);
    std::cout << "Sumtor " << rs << std::endl;
    std::cout << "Sumtoc " << cs << std::endl;
}


int main()
{
    Matrix empty(0,0);
    assert(empty.get_row_count() == 0);
    assert(empty.get_coloumn_count() == 0);

    Matrix square(2,2);
    assert(square.get_coloumn_count() == 2);
    assert(square.get_row_count() == 2);

    Matrix rectangluar(5,9);
    assert(rectangluar.get_row_count() == 5);
    assert(rectangluar.get_coloumn_count() == 9);

    rectangluar(2,3) = 5.0;
    assert(rectangluar(2,3) == 5.0);

    assert(rectangluar(2,3) == 5.0);
    test_copy();
    test_sum_speed();
    return 0;
}
