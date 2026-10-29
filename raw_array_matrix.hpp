#pragma once

#include <cstddef>
#include <utility>

template <typename T>
class RawArrayMatrix {
public:
    RawArrayMatrix(std::size_t rows, std::size_t cols)
        : rows_(rows), cols_(cols), elements_(new T[rows * cols]{}) {}

    ~RawArrayMatrix() { delete[] elements_; }

    RawArrayMatrix(const RawArrayMatrix& other)
        : rows_(other.rows_), cols_(other.cols_), elements_(new T[rows_ * cols_]) {
        for (std::size_t i = 0; i < rows_ * cols_; ++i) {
            elements_[i] = other.elements_[i];
        }
    }

    RawArrayMatrix& operator=(const RawArrayMatrix& other) {
        if (this == &other) {
            return *this;
        }
        RawArrayMatrix copy(other);
        swap(copy);
        return *this;
    }

    RawArrayMatrix(RawArrayMatrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_), elements_(other.elements_) {
        other.rows_ = 0;
        other.cols_ = 0;
        other.elements_ = nullptr;
    }

    RawArrayMatrix& operator=(RawArrayMatrix&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] elements_;
        rows_ = other.rows_;
        cols_ = other.cols_;
        elements_ = other.elements_;
        other.rows_ = 0;
        other.cols_ = 0;
        other.elements_ = nullptr;
        return *this;
    }

    T& operator()(std::size_t row, std::size_t col) {
        return elements_[row * cols_ + col];
    }

    const T& operator()(std::size_t row, std::size_t col) const {
        return elements_[row * cols_ + col];
    }

    std::size_t get_row_count() const { return rows_; }
    std::size_t get_coloumn_count() const { return cols_; }

    void swap(RawArrayMatrix& other) noexcept {
        std::swap(rows_, other.rows_);
        std::swap(cols_, other.cols_);
        std::swap(elements_, other.elements_);
    }

private:
    std::size_t rows_;
    std::size_t cols_;
    T* elements_;
};
