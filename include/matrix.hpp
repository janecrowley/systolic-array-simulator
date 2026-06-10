#ifndef MATRIX_H
#define MATRIX_H

#include <cstdint>
#include <vector>
#include <cstddef>

class Matrix {
    public:
        Matrix(size_t rows, size_t cols);

        int32_t& operator()(size_t row, size_t col);
        int32_t operator()(size_t row, size_t col) const;

        size_t rows() const;
        size_t cols() const;

    private:
        size_t rows_;
        size_t cols_;
        std::vector<int32_t> data_;
};
#endif