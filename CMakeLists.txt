cmake_minimum_required(VERSION 3.20)
project(matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MATRIX_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

add_executable(matrix_tests
    tests.cpp
    tests/matrix_tests.cpp
    tests/arithmetic_tests.cpp
    tests/solver_tests.cpp
    tests/lu_tests.cpp
    tests/benchmark_tests.cpp
    tests/test_declarations.hpp
    tests/test_helpers.hpp
    lu.hpp
    lu.tpp
    matrix.hpp
    matrix.tpp
    raw_array_matrix.hpp
    matrix_algorithms.hpp
    matrix_algorithms.tpp
)

set_target_properties(matrix_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_compile_options(matrix_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_options(matrix_tests PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<BOOL:${MATRIX_ENABLE_SANITIZERS}>:-fsanitize=address,undefined>
    $<$<CONFIG:Release>:-O3 -march=native>
)

target_link_options(matrix_tests PRIVATE
    $<$<BOOL:${MATRIX_ENABLE_SANITIZERS}>:-fsanitize=address,undefined>
)

enable_testing()
add_test(NAME matrix_tests COMMAND matrix_tests)
