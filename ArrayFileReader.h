//
// Created by Matthew Anderson on 6/2/16.
//

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#ifndef READ_FROM_DISK_ARRAYFILEREADER_H
#define READ_FROM_DISK_ARRAYFILEREADER_H

template <typename T>
class ArrayFileReader {

private:

    char mDelimiter;
    unsigned long mNumberOfLines;
    FILE *pFile;
    std::vector<unsigned long> mIndex;

    void buildFileIndices();

    void clearBuffer(char buffer[], const size_t bufferSize);

public:

    ArrayFileReader(const char *fileName, const char delimiter);

    ~ArrayFileReader();

    // linNum indexed starting at 0
    std::vector<T> getLine(unsigned long lineNum);

    size_t getNumberOfLines();
};

#endif //READ_FROM_DISK_ARRAYFILEREADER_H
