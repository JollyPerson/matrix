# Matrix

This is my C++20 matrix library project. I built it to understand what is
actually happening inside a matrix class: how the data is stored, how
operators work, why loop order affects performance, and why numerical
algorithms need to be written carefully.

The library stores matrix values in one flat, row-major `std::vector<T>`. It
supports matrix arithmetic, transposition, Gaussian elimination with partial
pivoting, and LU factorisation.

## What it supports

`Matrix<T>` currently includes:

- Construction with a specified number of rows and columns.
- Nested initializer-list construction.
- Element access with `operator()`.
- Bounds-checked access with `at`.
- Copy and move construction and assignment.
- Addition, subtraction, unary minus, and scalar operations.
- Matrix multiplication.
- Transpose and identity matrices.
- Stream output.
- Gaussian elimination with partial pivoting.
- Residual calculation for checking solutions.
- LU factorisation, solves, determinants, and inverses.

Example:

```cpp
Matrix<double> a{
    {1.0, 2.0},
    {3.0, 4.0}
};

Matrix<double> identity = Matrix<double>::identity(2);
Matrix<double> result = a * identity;
Matrix<double> transpose = a.transpose();
```

The matrix uses zero-based indexing. Its storage is row-major, so:

```text
1 2 3
4 5 6
```

is stored as:

```text
1 2 3 4 5 6
```

## Building and running

The normal macOS Debug and Release builds use GCC 15.

### Debug

```bash
cmake --preset debug-macos
cmake --build --preset debug-macos
./build/Debug/matrix_tests
```

### Release

```bash
cmake --preset release-macos
cmake --build --preset release-macos
./build/Release/matrix_tests
```

### Sanitizers

The sanitizer build uses Apple Clang. GCC works for the normal builds, but
the GCC sanitizer runtime crashed during pthread initialisation on my Mac,
before the test program reached `main`.

```bash
cmake --preset sanitize-macos
cmake --build --preset sanitize-macos
./build/Sanitize/matrix_tests
```

This enables AddressSanitizer and UndefinedBehaviorSanitizer. CTest can run
the executable as well:

```bash
ctest --test-dir build/Sanitize --output-on-failure
ctest --test-dir build/Release --output-on-failure
```

The compiler is cached by CMake. If I switch compilers, I should use a new
build directory or reconfigure the existing one properly.

## Project structure

- `matrix.hpp` and `matrix.tpp`: the main matrix class.
- `matrix_algorithms.hpp` and `matrix_algorithms.tpp`: operators,
  multiplication, solving, residuals, and approximate comparison.
- `lu.hpp` and `lu.tpp`: LU factorisation and related operations.
- `raw_array_matrix.hpp`: raw-array implementation used in the storage
  comparison.
- `tests.cpp`: test-suite entry point.
- `tests/`: matrix, arithmetic, solver, LU, and benchmark tests.
- `CMakeLists.txt`: build target and compiler options.
- `CMakePresets.json`: GCC and Clang build presets.


## Design decisions

### Flat row-major storage

The matrix uses one `std::vector<T>` instead of a separate allocation for
each row. The position of `(row, column)` is:

```text
row * number_of_columns + column
```

This gives one owner, one allocation, straightforward copying and moving, and
good cache behaviour for row-wise operations.

### Multiplication loop order

The multiplication implementation uses the `i-k-j` loop order. Once `i` and
`k` are fixed, it walks across a row of the right-hand matrix. Those values
are next to each other in row-major storage, so this is more cache-friendly
than the simple `i-j-k` version.

### Gaussian elimination and pivoting

The solver handles systems of the form:

```text
A x = b
```

It checks the input shapes, chooses a pivot, swaps rows when needed,
eliminates values below the pivot, and finishes with back substitution.

Partial pivoting chooses the largest available value in the current column.
This reduces the damage caused by dividing by a very small pivot. Singular
and nearly singular systems are rejected using a tolerance.

I solve systems directly instead of calculating an inverse first. Computing
an inverse does extra work when only one solution is needed and is generally
not the best numerical approach.

### Rule of zero

The matrix owns its data through `std::vector`, so the class does not need to
manually implement the destructor, copy operations, or move operations.
The standard container already handles those responsibilities.

The `MatrixElement` concept is based on the operations needed by matrix
arithmetic. This allows numeric-like user-defined types, not only built-in
integers and floating-point types.

### Rule of five: the raw-array example

The project also includes `RawArrayMatrix`, which stores its values in a
manually allocated `T*`. This is a useful comparison because a raw pointer
does not manage ownership by itself. The class has to implement the rule of
five:

1. **Destructor**: calls `delete[]` so the allocation is released.
2. **Copy constructor**: allocates a new array and copies every element,
   giving the new matrix its own data.
3. **Copy assignment**: handles self-assignment, makes a copy, and swaps the
   resources safely.
4. **Move constructor**: takes the existing pointer instead of copying all
   elements, then leaves the source with a null pointer.
5. **Move assignment**: releases the current allocation, takes ownership of
   the other matrix's pointer, and resets the source.

Without these functions, copying two raw-array matrices would copy only the
pointer. Both objects would then refer to the same allocation, which could
cause changes to leak between objects and eventually cause a double
`delete[]`. The vector-based `Matrix` avoids this manual ownership code, which
is why it follows the rule of zero instead.

## Testing

The test suite covers:

- Normal construction and element access.
- Empty, square, and rectangular matrices.
- Bounds errors and shape mismatches.
- Initializer-list errors.
- Copying, moving, self-assignment, and vector growth.
- Arithmetic and transpose identities.
- Matrix multiplication.
- Singular and nearly singular systems.
- Partial pivoting.
- Fixed-seed random systems.
- Hilbert matrices.
- LU reconstruction, solving, determinant, and inverse operations.
- Both `float` and `double`.

The solver tests use residuals:

```text
r = b - A*x
```

The largest absolute value in `r` is used as the residual. This checks how
well the calculated answer satisfies the original equations. Hilbert-matrix
tests also show why a small residual does not always guarantee a small error
in the answer itself.

The tests include a `CopyCountingNumber` type that records copies and moves
while evaluating expressions such as:

```cpp
Matrix<CopyCountingNumber> result = a + b + c;
```

## Measurements

The test executable prints experiments for:

- Row-major versus column-major traversal.
- `i-j-k` versus `i-k-j` multiplication.
- Reusing an LU factorisation versus repeatedly solving from scratch.
- `std::vector` versus raw-array storage.
- `float` versus `double` on Hilbert matrices.

These are simple experiments rather than a full benchmarking suite. Results
depend on the processor, compiler, optimisation flags, matrix size, and other
programs running at the same time.

One Release run used GCC 15.3.0 with `-O3 -march=native` on my x86_64
`MacBookPro16,1`. These times are in microseconds and come from one run:

| Experiment | Result |
| --- | ---: |
| Row-major traversal | 25,093 |
| Column-major traversal | 140,667 |
| Row/column speedup | 5.61x |
| Multiplication, `i-j-k` | 10,723 |
| Multiplication, `i-k-j` | 1,594 |
| Multiplication speedup | 6.73x |
| Ten full Gaussian solves | 11,039 |
| LU factorisation plus ten solves | 2,114 |
| LU speedup | 5.22x |
| `std::vector` traversal | 1,240 |
| Raw-array traversal | 1,058 |

For a more reliable comparison, I would run each benchmark many times and
report a median rather than relying on one run.

## Limitations and future work

This is a dense matrix library. It does not currently include:

- Sparse matrices.
- Blocked multiplication.
- Hand-written SIMD.
- A BLAS backend.
- A dedicated benchmark framework.

It is mainly a learning and experimentation project, so it will be slower
than a library designed specifically for high-performance linear algebra.

Possible future improvements include better benchmark isolation, median-based
reports, more one-by-one and empty-matrix tests, improved naming such as
`get_column_count`, and blocked or BLAS-backed multiplication.
