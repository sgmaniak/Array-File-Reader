//
// Created by Matthew Anderson on 6/2/16.
//

#include "ArrayFileReader.h"

// ==================================== PRIVATE

template <typename T>
void ArrayFileReader<T>::buildFileIndices(){

    //identify if error opening file
    if (pFile==NULL) perror ("Error opening file");

    //set pFile at beginning of file
    fseek(pFile, 0, SEEK_SET);

    //solve fence-post problem, put beginning of file at index 0
    mIndex.push_back(ftell(pFile));
    char c = (char)fgetc(pFile);

    //for every newline add file position to mIndex
    while(c != EOF) {
        if ((int)c == 10) {
            mIndex.push_back(ftell(pFile));

            //count number of lines in file simultaneously
            mNumberOfLines++;
        }
        c = (char)fgetc(pFile);
    }
}

//reset buffer to white space to overwrite previous double held
template <typename T>
void ArrayFileReader<T>::clearBuffer(char buffer[], const size_t bufferSize){
    for (size_t i = 0; i < bufferSize; i++) {
        buffer[i] = ' ';
    }
}

// ==================================== PUBLIC

template <typename T>
ArrayFileReader<T>::ArrayFileReader(const char *fileName, const char delimiter) {
    mDelimiter = delimiter;
    mNumberOfLines = 0;
    pFile = fopen (fileName,"rb");
    buildFileIndices();
}

template <typename T>
ArrayFileReader<T>::~ArrayFileReader() {
    fseek(pFile, 0, SEEK_SET);
    fclose(pFile);
    pFile = nullptr;
}

template <typename T>
std::vector<T> ArrayFileReader<T>::getLine(unsigned long lineNum){
    if (lineNum >= mNumberOfLines) {
        throw std::invalid_argument("Requested invalid line number " + lineNum);
    }
    fseek(pFile, mIndex[lineNum], SEEK_SET);
    const size_t bufferSize = 50;
    char buffer[bufferSize];
    char c = (char)fgetc(pFile);
    std::vector<T> row;
    while (int(c) != 10 && c != EOF) {
        size_t i = 0;
        while (c != mDelimiter && (int)c != 10 && c != EOF) {
            buffer[i] = c;
            c = (char)fgetc(pFile);
            i++;
        }
        if (c == mDelimiter) c = (char)fgetc(pFile);
        //parse double and append to row
        row.push_back(atof(buffer));
        clearBuffer(buffer, bufferSize);
    }
    return row;
}

template <typename T>
size_t ArrayFileReader<T>::getNumberOfLines() {
    return mNumberOfLines;
}

template class ArrayFileReader<double>;
template class ArrayFileReader<int>;