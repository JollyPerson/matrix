#include "test_helpers.hpp"

volatile double benchmark_sink = 0.0;

template <typename MatrixType>
double benchmark_sum(const MatrixType& matrix) {
    double sum = 0.0;
    for (std::size_t row = 0; row < matrix.get_row_count(); ++row) {
        for (std::size_t col = 0; col < matrix.get_coloumn_count(); ++col) {
            sum += matrix(row, col);
        }
    }
    return sum;
}

void benchmark_vector_vs_raw_array() {
    constexpr std::size_t size = 800;
    Matrix<double> vector_matrix(size, size);
    RawArrayMatrix<double> raw_matrix(size, size);

    for (std::size_t row = 0; row < size; ++row) {
        for (std::size_t col = 0; col < size; ++col) {
            const double value = static_cast<double>((row * size + col) % 17);
            vector_matrix(row, col) = value;
            raw_matrix(row, col) = value;
        }
    }

    const auto vector_start = std::chrono::high_resolution_clock::now();
    const double vector_sum = benchmark_sum(vector_matrix);
    const auto vector_end = std::chrono::high_resolution_clock::now();

    const auto raw_start = std::chrono::high_resolution_clock::now();
    const double raw_sum = benchmark_sum(raw_matrix);
    const auto raw_end = std::chrono::high_resolution_clock::now();

    assert(vector_sum == raw_sum);
    const auto vector_duration =
        std::chrono::duration_cast<std::chrono::microseconds>(vector_end - vector_start);
    const auto raw_duration =
        std::chrono::duration_cast<std::chrono::microseconds>(raw_end - raw_start);

    benchmark_sink = vector_sum + raw_sum;
    std::cout << "Vector versus raw-array benchmark:\n";
    std::cout << "  Matrix size                 : " << size << "x" << size << "\n";
    std::cout << "  std::vector storage         : " << vector_duration.count() << " micros\n";
    std::cout << "  raw array storage           : " << raw_duration.count() << " micros\n";
}
void test_sum_speed() {
    auto start = std::chrono::high_resolution_clock::now();
    const Matrix<double> a = generate_matrix<double>(42, 4000, 4000);
    const Matrix<double> b = generate_matrix<double>(43, 4000, 4000);

    // 1. Row-by-row traversal (Stride 1 - Fast)
    auto t0 = std::chrono::high_resolution_clock::now();
    double row_sum = sum_matrix_row(a);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto row_duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    // 2. Col-by-col traversal (Stride cols - Slow)
    auto t2 = std::chrono::high_resolution_clock::now();
    double col_sum = sum_matrix_coloumn(b);
    auto t3 = std::chrono::high_resolution_clock::now();
    auto col_duration = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);

    auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(t3 - start);

    std::cout << "Sum speed benchmark:\n";
    std::cout << "  Row-by-row (stride 1)    : " << row_duration.count() << " micros (sum = " << row_sum << ")\n";
    std::cout << "  Col-by-col (stride cols) : " << col_duration.count() << " micros (sum = " << col_sum << ")\n";
    std::cout << "  Total                    : " << total_duration.count() << " micros\n";
    double speedup = row_duration.count() > 0 ? static_cast<double>(col_duration.count()) / static_cast<double>(row_duration.count()) : 0.0;

    std::cout << "  Speedup                  : " << std::fixed << std::setprecision(2) << speedup << "x\n";

}

// ==========================================
// Constuctors & Element Access
// ==========================================

void benchmark_fast_slow_multi() {
    std::cout << "Multiplication speed benchmark:\n";
    const size_t n = 200;
    const Matrix<double> a = generate_matrix<double>(1, n, n);
    const Matrix<double> b = generate_matrix<double>(2, n, n);

    auto start_slow = std::chrono::high_resolution_clock::now();
    Matrix<double> slow_res = matrix::slow_multiply(a, b);
    auto end_slow = std::chrono::high_resolution_clock::now();

    auto start_fast = std::chrono::high_resolution_clock::now();
    Matrix<double> fast_res = a * b;
    auto end_fast = std::chrono::high_resolution_clock::now();

    assert(matrix::approx_equal(slow_res, fast_res));

    auto slow_dur = std::chrono::duration_cast<std::chrono::microseconds>(end_slow - start_slow);
    auto fast_dur = std::chrono::duration_cast<std::chrono::microseconds>(end_fast - start_fast);


    std::cout << "  Matrix<double> size              : " << n << "x" << n << "\n";
    std::cout << "  Slow multiply (i-j-k)    : " << slow_dur.count() << " micros\n";
    std::cout << "  Fast multiply (i-k-j)    : " << fast_dur.count() << " micros\n";
    double speedup = fast_dur.count() > 0 ? static_cast<double>(slow_dur.count()) / static_cast<double>(fast_dur.count()) : 0.0;

    std::cout << "  Speedup                  : " << std::fixed << std::setprecision(2) << speedup << "x\n";
}
