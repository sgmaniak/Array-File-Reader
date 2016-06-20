//
// Created by Matthew Anderson on 6/9/16.
//


#include "ArrayStreamReader.h"

void ArrayStreamReader::clearBuffer(char buffer[], const size_t bufferSize){
    for (size_t i = 0; i < bufferSize; i++) {
        buffer[i] = ' ';
    }
}

ArrayStreamReader::ArrayStreamReader(const char *fileName, const char delimiter) {
    myFile.open(fileName);
    std::string line;
    mNumberLines = 0;
    mDelimiter = delimiter;
    while(!myFile.eof()){
        getline(myFile,line);
        mNumberLines++;
    }
}


ArrayStreamReader::~ArrayStreamReader() {
    myFile.close();
}

std::vector<double> ArrayStreamReader::getLine(size_t lineNum){
    if (lineNum >= mNumberLines) {
        throw std::invalid_argument("Requested invalid line number " + lineNum);
    }

    std::string line;
    for (size_t k = 0; k < lineNum; k++) {
        getline(myFile, line);
    }

    const size_t bufferSize = 50;
    char buffer[bufferSize];
    size_t index = 0;
    char c = line[index];
    std::vector<double> row;
    while (int(c) != 10 && c != EOF) {
        size_t i = 0;
        while (c != mDelimiter && (int)c != 10 && c != EOF) {
            buffer[i] = c;
            index++;
            c = line[index];
            i++;
        }
        if (c == mDelimiter) {
            index++;
            c = line[index];
        }
        //parse double and append to row
        row.push_back(atof(buffer));
        clearBuffer(buffer, bufferSize);
    }
    return row;
}

size_t ArrayStreamReader::getNumberOfLines() {
    return mNumberLines;
}