//
// Created by anderson on 6/9/16.
//

#include <fstream>
#include <iostream>

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int main () {
    srand(713);
    size_t numColumns = 50;
    size_t numLines =   1000000;
    std::string fName = "1Mx50.txt";
    std::ofstream myFile;
    myFile.open (fName);
    for (size_t i = 0; i < numLines; i++) {
        for (size_t j = 0; j < numColumns; j++) {
            myFile << fRand(0, 10000) << " ";
        }
        myFile << std::endl;
        if (i % 10000 == 0) std::cout << i << std::endl;
    }
    myFile.close();
    std::cout << "done!";
    return 0;
}
