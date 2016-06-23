//
// Created by Matthew Anderson 6/13/16

#ifndef CONVERT_DOUBLE_TO_BINARY_BINARYFILEPARSER_H
#define CONVERT_DOUBLE_TO_BINARY_BINARYFILEPARSER_H


#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "Timer.h"

class BinaryFileParser {

    size_t OUT_FILE_SIZE = 100000;

private:
    char mDelimiter;
    unsigned long mNumberOfLines;
    std::vector<std::vector<long>> mIndex;
    std::vector<unsigned long> mElementsInRow;
    std::vector<const char*> mFileList;

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

    std::vector<double> getLine(unsigned long lineNum);

    unsigned long getNumberOfLines() {return mNumberOfLines;}

};

#endif //CONVERT_DOUBLE_TO_BINARY_BINARYFILEPARSER_H

