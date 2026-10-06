#include <assert.h>
#include <iostream>

#include "matrix.hpp"
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

    const Matrix& fixe = rectangluar;

    assert(rectangluar(2,3) == 5.0);

    rectangluar(10,3) = 1.0;

    return 0;
}
