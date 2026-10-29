# Matrix

This is my C++20 matrix project. It stores matrix values in one flat
row-major `std::vector` and currently includes arithmetic, transpose,
Gaussian elimination, and LU factorisation.

The main purpose of the project was to understand how a matrix class works
underneath instead of treating it as a black box. I wanted to see the
difference between a raw array and a vector, understand why the order of
loops matters, and get more comfortable with copying, moving, templates, and
numerical errors.

## What the library can do

The `Matrix<T>` class supports:

- Construction with a chosen number of rows and columns.
- Construction from nested initializer lists.
- Element access with `operator()` and bounds-checked access with `at`.
- Copy construction, copy assignment, move construction, and move assignment.
- Addition, subtraction, unary minus, and scalar multiplication/division.
- Matrix multiplication.
- Transpose and identity matrices.
- Stream output with `operator<<`.
- Gaussian elimination with partial pivoting for floating-point matrices.
- Residual calculation for checking how well a solution satisfies `Ax = b`.
- LU factorisation, solving, determinants, and inverses through the LU code.

The matrix uses zero-based indexes, so the first element is accessed with
`matrix(0, 0)`. The storage is row-major, which means a matrix such as:

```text
1 2 3
4 5 6
```

is stored in memory in the order `1, 2, 3, 4, 5, 6`.

For example, basic matrix operations look like this:

```cpp
Matrix<double> a{
    {1.0, 2.0},
    {3.0, 4.0}
};

Matrix<double> b = Matrix<double>::identity(2);
Matrix<double> result = a * b;
Matrix<double> transposed = a.transpose();
```

## Building and running it

I normally build the project with GCC 15:

```bash
cmake --preset debug-macos
cmake --build --preset debug-macos
./build/Debug/matrix_tests
```

For the optimised build:

```bash
cmake --preset release-macos
cmake --build --preset release-macos
./build/Release/matrix_tests
```

I use Apple Clang for the sanitizer build. GCC worked for the normal builds,
but its sanitizer runtime crashed during pthread setup on my Mac before the
test program even reached `main`.

```bash
cmake --preset sanitize-macos
cmake --build --preset sanitize-macos
./build/Sanitize/matrix_tests
```

The sanitizer preset runs AddressSanitizer and UndefinedBehaviorSanitizer.
The same test can also be run through CTest:

```bash
ctest --test-dir build/Sanitize --output-on-failure
```

The build presets are defined in `CMakePresets.json`. The normal Debug and
Release presets use the Homebrew GCC installation, while the sanitizer preset
uses `/usr/bin/clang++`. Separate build directories are used because CMake
remembers which compiler was used to configure a build.

## Project layout

The important files are:

- `matrix.hpp` and `matrix.tpp` contain the matrix class.
- `matrix_algorithms.hpp` and `matrix_algorithms.tpp` contain the free
  operators and algorithms.
- `lu.hpp` and `lu.tpp` contain the LU implementation.
- `raw_array_matrix.hpp` contains the raw-array version used for comparison.
- `tests.cpp` is the test-suite entry point.
- `tests/` contains the matrix, arithmetic, solver, LU, and benchmark tests.
- `CMakeLists.txt` and `CMakePresets.json` contain the build setup.

## Some of the choices I made

- I used a flat, row-major vector because it keeps ownership simple and makes
  row-by-row access cache friendly.
- Matrix operations check their dimensions and throw
  `std::invalid_argument` when the shapes do not match.
- Matrix multiplication uses the `i-k-j` loop order. This means the values
  from a row of the right-hand matrix are read in order.
- The solver uses Gaussian elimination with partial pivoting. I did not
  implement solving by calculating an inverse because that does extra work and
  is less suitable numerically.
- The class mostly follows the rule of zero. The vector handles the memory,
  copying, and moving.
- The solver tests use residuals and fixed random seeds, so the results can be
  checked and reproduced.

### Why the storage is flat

I chose one `std::vector<T>` instead of a vector of rows. A flat vector means
there is one allocation and the values are next to each other in memory. It
also makes the position of an element easy to calculate:

```text
index = row * number_of_columns + column
```

This is useful for row-by-row operations because neighbouring values are read
next to each other. It also makes copying and moving the matrix simpler because
the vector already knows how to manage its storage.

### Why pivoting is used

During Gaussian elimination, the current diagonal value is used as a pivot.
If that value is very small, dividing by it can make rounding errors much
worse. Partial pivoting searches the rows below for a larger value in the same
column and swaps that row into place.

The solver rejects singular or nearly singular systems when the pivot is below
its tolerance. This is better than returning a result that looks valid but is
mostly numerical noise.

### Why the residual is useful

For a system `Ax = b`, the residual is:

```text
r = b - Ax
```

The tests use the largest absolute value in `r` as the residual. A small
residual means the computed answer satisfies the original equations closely.
This is more useful for general solver tests than comparing every answer with
one manually typed value.

## Tests and experiments

The test program covers normal cases, invalid shapes, singular systems,
partial pivoting, random systems, Hilbert matrices, LU factorisation, and
both `float` and `double`.

It also prints a few performance experiments. These numbers are only from my
machine, so they should not be treated as universal results.

The following is one Release run using GCC 15.3.0, `-O3 -march=native`, on my
x86_64 `MacBookPro16,1`. Times are in microseconds, and this was only one run:

| Experiment | Time |
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

The tests also compare `float` and `double` on Hilbert matrices and check
epsilon-sized pivots and nearly singular systems.

### Copy and move testing

There are tests for copying a matrix, assigning one matrix to another, moving
a matrix, and growing a `std::vector<Matrix<double>>`. The vector-growth test
checks that the matrix elements are moved instead of unnecessarily copied.

The project also includes a small `CopyCountingNumber` test type. It records
copy and move operations while evaluating an expression such as:

```cpp
Matrix<CopyCountingNumber> result = a + b + c;
```

This is useful because operator expressions can accidentally create more
temporary copies than expected.

### Numerical experiments

The solver tests include:

- A tiny pivot case where pivoting changes the result substantially.
- Random matrices generated with fixed seeds.
- Hilbert matrices, which become increasingly ill-conditioned as their size
  grows.
- A comparison between `float` and `double`.
- Singular and almost-singular systems.
- LU factorisation reused for several right-hand sides.

The Hilbert matrix test is especially useful for showing that a small residual
does not always mean the answer is accurate. An ill-conditioned system can
still have a small residual while the calculated values differ from the true
values.

### Benchmark notes

The benchmark output is included in the test executable so it is easy to run,
but it is not a formal benchmarking framework. For a more reliable comparison
I would run each experiment multiple times and report the median rather than
relying on one run. The timings are affected by the processor, compiler,
optimisation flags, background processes, and matrix size.

## Limitations

This is a dense matrix class. It does not have sparse matrices, blocked
multiplication, hand-written SIMD, or a BLAS backend, so it will be slower
than a library designed specifically for high-performance linear algebra.

The `MatrixElement` concept is based on the operations needed by the matrix
class instead of only accepting built-in numeric types. This lets me use
numeric-like test types as well. For example, the tests include a
`CopyCountingNumber` that counts copies and moves while evaluating `a + b + c`.
