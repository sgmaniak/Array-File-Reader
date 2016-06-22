//
// Created by Matthew Anderson 6/13/16

#ifndef CONVERT_DOUBLE_TO_BINARY_BINARYFILEPARSER_H
#define CONVERT_DOUBLE_TO_BINARY_BINARYFILEPARSER_H


#include <cstdio>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "Timer.h"

class BinaryFileParser {

private:
    char mDelimiter;
    unsigned long mNumberOfLines;
    std::vector<long> mIndex;
    std::vector<unsigned long> mElementsInRow;

    // for testing only **********
    Timer tTimer;




    template <typename T>
    void clearBuffer(T buffer[], const size_t bufferSize);

    double* parseCharToDouble(const std::vector<char> &c, const size_t size);

    void parse(const char *inFileName, const char *outFileName);

public:
    // for testing only **********
    double tReadTime;
    double tSeekTime;
    double tArrayTime;

    //***********
    //NOTE: as of now, valid file format has delimiting character at the end of each line
    //**********

    BinaryFileParser(const char *inFileName, const char *outputFileName, const char delimiter);

    std::vector<double> getLine(unsigned long lineNum, std::ifstream &is);

    unsigned long getNumberOfLines() {return mNumberOfLines;}

};

#endif //CONVERT_DOUBLE_TO_BINARY_BINARYFILEPARSER_H

