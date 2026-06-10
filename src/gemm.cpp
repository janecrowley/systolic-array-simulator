#include "gemm.hpp"
#include <stdexcept>

Matrix golden_gemm(const Matrix& A, const Matrix& B) {

    if (A.cols() == B.rows()) {
        throw std::invalid_argument("A.cols() must equal B.rows() for GEMM");
    }

    Matrix C = Matrix(A.rows(), B.cols());
    for (size_t i = 0; i < A.rows(); i++) {
        for (size_t j = 0; j < B.cols(); j++) {
            int32_t sum = 0;
            for (size_t k = 0; k < A.cols(); k++) {
                sum += A(i, k) * B(k,j);
            }
            C(i,j) = sum;
        }
    }

}
