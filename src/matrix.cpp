#include "matrix.hpp"

Matrix::Matrix(size_t rows, size_t cols) 
    :   rows_(rows),
        cols_(cols),
        data_(rows * cols, 0)
{
}

int32_t& Matrix::operator()(size_t row, size_t col) {
    return data_[row * cols_ + col];
}

int32_t Matrix::operator()(size_t row, size_t col) const {
    return data_[row * cols_ + col];
}

size_t Matrix::rows() const {
    return rows_;
}

size_t Matrix::cols() const {
    return cols_;
}