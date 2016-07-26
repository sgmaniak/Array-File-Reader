//
// Created by anderson on 7/25/16.
//

#include "sarray2dfromfile.h"

template <typename T>
void printLines(T *t, ulong size) {
    for (size_t i = 0; i < size; i++) {
        std::cout << t[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    try {
        SArray2dFromFile<double> sArray2dFromFile("testinputsmall_double_sparse.txt");
        std::cout << "Num rows: " << sArray2dFromFile.get_num_rows() << std::endl;
        for (int i = 0; i < sArray2dFromFile.get_num_rows(); ++i) {
            std::cout << "row: " << i << std::endl;
            double *row = sArray2dFromFile.get_row(i);
            printLines(row, sArray2dFromFile.get_num_cols());
            delete row;
        }
    }catch(std::exception &e) {
        std::cout << e.what();
    }
    return 0;
}