#include "../matrix.h"
#include <iostream>
#include <iomanip>

int main() {
    Matrix<int> matrix(10, 20);
    for (int i = 0; i < matrix.m(); ++i) {
        for (int j = 0; j < matrix.n(); ++j) {
            matrix.at(i, j) = i * 100 + j;
        }
    }
    for (int i = 0; i < matrix.m(); ++i) {
        for (int j = 0; j < matrix.n(); ++j) {
            std::cout << std::setw(3) << std::setfill('0') << matrix.at(i, j) << " ";
        }
        std::cout << std::endl;
    }
}