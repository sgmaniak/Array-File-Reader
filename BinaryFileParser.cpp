#include <sstream>
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
    mFileList.push_back(outFileName);
    std::vector<char> line;
    std::vector<long> currentIndex;
    unsigned long lineSize = 0;
    char c;

    do {
        c = (char)fgetc(inFile);
        line.push_back(c);
        if (c == mDelimiter) lineSize++;
        if ((int)c == 10 || c == EOF) {
            currentIndex.push_back((long)os.tellp());
            mElementsInRow.push_back(lineSize);
            os.write(reinterpret_cast<const char*>(parseCharToDouble(line, lineSize)),
                     std::streamsize(lineSize*sizeof(double)));
            mNumberOfLines++;
            lineSize = 0;
            line.clear();
            if(mNumberOfLines % OUT_FILE_SIZE == 0 && c != EOF) {
                os.close();
                mIndex.push_back(currentIndex);
                std::ostringstream oss;
                oss << mFileList.size() << "-";
                std::string newFName = oss.str() + outFileName;
                os.open(newFName, std::ios::binary | std::ios::out);
                mFileList.push_back(newFName);
                currentIndex.clear();
            }
        }
    } while(c != EOF);
    if (mNumberOfLines < OUT_FILE_SIZE) {
        mIndex.push_back(currentIndex);
    }
    fclose(inFile);
    os.close();
}

BinaryFileParser::BinaryFileParser(const char *inFileName, const char *outputFileName, const char delimiter){
    mNumberOfLines = 0;
    tReadTime = 0;
    tSeekTime = 0;
    tArrayTime = 0;
    tFileTime = 0;
    tCreateTime = 0;
    mDelimiter = delimiter;
    parse(inFileName, outputFileName);
}

BinaryFileParser::~BinaryFileParser() {
    for(size_t i = 0; i < mFileList.size(); i++) {
        if( remove(mFileList[i].c_str()) != 0 ) {
            perror("Error deleting file");
            std::cout << mFileList[i] << std::endl;
        }
    }
}

std::vector<double> BinaryFileParser::getLine(unsigned long lineNum) {
    if (lineNum >= mNumberOfLines) {
        throw std::invalid_argument("Requested invalid line number " + lineNum);
    }
    tTimer.start();
    std::ifstream is(mFileList[lineNum / OUT_FILE_SIZE], std::ios::binary | std::ios::in);
    tFileTime += tTimer.stop();
    tTimer.start();
    is.seekg(mIndex[lineNum / OUT_FILE_SIZE][lineNum % OUT_FILE_SIZE]);
    tSeekTime += tTimer.stop();
    tTimer.start();
    std::vector<double> line;
    double *row = new double[mElementsInRow[lineNum]];
    tCreateTime += tTimer.stop();
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