#include "BinaryFileParser.h"

template <typename T>
void BinaryFileParser::clearBuffer(T buffer[], const size_t bufferSize){
    for (size_t i = 0; i < bufferSize; i++) {
        buffer[i] = 0;
    }
}


double* BinaryFileParser::parseCharToDouble(const std::vector<char> &c, const size_t size) {
    double *d = new double[size];
    size_t j = 0;
    size_t bufferSize = 50;
    char buffer[bufferSize];
    for (size_t i = 0; i < size; i++) {
        size_t bufferIndex = 0;
        while(c[j] != mDelimiter && c[j] != 10 && c[j] != EOF) {
            buffer[bufferIndex] = c[j];
            j++;
            bufferIndex++;
        }
        d[i] = atof(buffer);
        j++;
        clearBuffer(buffer, bufferSize);
    }
    return d;
}

void BinaryFileParser::parse(const char *inFileName, const char *outFileName) {
    FILE *inFile;
    inFile = fopen(inFileName, "rb");
    if (inFile==NULL) perror ("Error opening input file");
    std::ofstream os(outFileName, std::ios::binary | std::ios::out);
    std::vector<char> line;
    unsigned long lineSize = 0;
    char c;

    do {
        c = (char)fgetc(inFile);
        line.push_back(c);
        if (c == mDelimiter) lineSize++;
        if ((int)c == 10 || c == EOF) {
            mIndex.push_back((long)os.tellp());
            mElementsInRow.push_back(lineSize);
            os.write(reinterpret_cast<const char*>(parseCharToDouble(line, lineSize)),
                     std::streamsize(lineSize*sizeof(double)));
            mNumberOfLines++;
            lineSize = 0;
            line.clear();
        }
    } while(c != EOF);
    fclose(inFile);
    os.close();
}

BinaryFileParser::BinaryFileParser(const char *inFileName, const char *outputFileName, const char delimiter){
    mNumberOfLines = 0;
    tReadTime = 0;
    tSeekTime = 0;
    tArrayTime = 0;
    mDelimiter = delimiter;
    parse(inFileName, outputFileName);
}

std::vector<double> BinaryFileParser::getLine(unsigned long lineNum, std::ifstream &is) {
    if (lineNum >= mNumberOfLines) {
        throw std::invalid_argument("Requested invalid line number " + lineNum);
    }
    tTimer.start();
    is.seekg(mIndex[lineNum]);
    tSeekTime += tTimer.stop();
    std::vector<double> line;
    double *row = new double[mElementsInRow[lineNum]];
    tTimer.start();
    is.read(reinterpret_cast<char*>(row), std::streamsize(mElementsInRow[lineNum]*sizeof(double)));
    tReadTime += tTimer.stop();
    tTimer.start();
    for(size_t i = 0; i <mElementsInRow[lineNum]; i++) {
        line.push_back(row[i]);
    }
    tArrayTime += tTimer.stop();
    delete row;
    return line;
}