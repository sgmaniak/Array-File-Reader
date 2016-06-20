//
// Created by anderson on 6/9/16.
//

#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

#ifndef READ_FROM_DISK_ARRAYSTREAMREADER_H
#define READ_FROM_DISK_ARRAYSTREAMREADER_H

class ArrayStreamReader {

private:

    std::ifstream myFile;
    size_t mNumberLines;
    char mDelimiter;

    void clearBuffer(char buffer[], const size_t bufferSize);

public:

    ArrayStreamReader(const char *fileName, const char delimiter);

    ~ArrayStreamReader();

    std::vector<double> getLine(size_t lineNum);

    size_t getNumberOfLines();
};


#endif //READ_FROM_DISK_ARRAYSTREAMREADER_H
