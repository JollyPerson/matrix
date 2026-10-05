cmake_minimum_required(VERSION 3.20)
project(matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(matrix_tests
    matrix.cpp
    tests.cpp
)

target_compile_options(matrix_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_options(matrix_tests PRIVATE
    $<$<CONFIG:Debug>:-g>
   # $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    $<$<CONFIG:Release>:-O2>
)

target_link_options(matrix_tests PRIVATE
   # $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

enable_testing()
add_test(NAME matrix_tests COMMAND matrix_tests)
