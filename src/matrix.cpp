#include "matrix.hpp"

Matrix::Matrix(int32_t rows, int32_t cols) 
    :   rows_(rows),
        cols_(cols),
        data_(rows * cols, 0)
{
}

Matrix::int32_t& operator()(size_t row, size_t col) {
    return data_(rows * cols + cols);
}

Matrix::int32_t operator()(size_t row, size_t col) const {
    return data_(rows * cols + cols);
}

Matrix::size_t rows() const {
    return rows_;
}

Matrix::size_t cols() const {
    return cols;
}