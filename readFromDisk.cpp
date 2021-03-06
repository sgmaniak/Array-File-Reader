//
// Created by anderson on 6/8/16.
//

#include "ArrayFileReader.h"
#include "BinaryFileParser.h"
#include "Timer.h"

void clearBuffer(char buffer[], const size_t bufferSize){
    for (size_t i = 0; i < bufferSize; i++) {
        buffer[i] = ' ';
    }
}

void printLines(const std::vector<double> &line) {
    for (size_t i = 0; i < line.size(); i++) {
        std::cout << line[i] << " ";
    }
    std::cout << std::endl;
}

std::vector<double> getLineOfFile(std::ifstream &myFile, size_t lineNum, size_t mNumberLines, char mDelimiter) {

    if (lineNum >= mNumberLines) {
        throw std::invalid_argument("Requested invalid line number " + lineNum);
    }

    std::string line;
    for (size_t i = 0; i < lineNum; i++) {
        std::getline(myFile, line);
    }

    const size_t bufferSize = 50;
    char buffer[bufferSize];
    std::vector<double> row;
    char c = (char) myFile.get();
    while (int(c) != 10 && c != EOF) {
        size_t i = 0;
        while (c != mDelimiter && (int) c != 10 && c != EOF) {
            buffer[i] = c;
            c = (char) myFile.get();
            i++;
        }
        if (c == mDelimiter) c = (char) myFile.get();
        row.push_back(atof(buffer));
        clearBuffer(buffer, bufferSize);
    }
    return row;
}





// **************************************************************************************************************

int main() {

    const char *fname = "5Mx50.txt";
    const char delimiter = ' ';
    size_t iterations = 1;
    double asrHandicap = 0.001;
    double afrHandicap = 0.1;
    double bfpHandicap = 0.001;
    bool verbose = true;
    Timer timer = Timer();
    const char *testType = "RUN";

    std::cout << "iterations: " << iterations << std::endl;
    std::cout << "BFR handicap: " << bfpHandicap << std::endl;


    ///////////////////////////////////////////


    if(testType == "SETUP") {
        std::cout << "SETUP:" << std::endl;
        std::cout << "ARRAY FILE STREAMER:" << std::endl;
        srand(181);
        timer.start();
        for(size_t i = 0; i < iterations; i++) {
            if (verbose) std::cout << "iteration :" << i + 1 << std::endl;
            std::ifstream myFile;
            myFile.open(fname);
            std::string line;
            size_t mNumberLines = 0;
            while (!myFile.eof()) {
                getline(myFile, line);
                mNumberLines++;
            }
        }
        double streamSetup = timer.stop();

        std::cout << "ARRAY FILE READER:" << std::endl;

        timer.start();

        for(size_t i = 0; i < iterations; i++) {
            if (verbose) std::cout << "iteration :" << i + 1 << std::endl;
            ArrayFileReader<double> arrayFileReader = ArrayFileReader<double>(fname, delimiter);
        }
        double readerSetup = timer.stop();

        std::cout << "Binary File Parser:" << std::endl;
        timer.start();
        for(size_t i = 0; i < iterations; i++) {
            if (verbose) std::cout << "iteration :" << i + 1 << std::endl;
            const char *fOutName = "myTestOut.txt";
            BinaryFileParser binaryFileParser = BinaryFileParser(fname, fOutName, delimiter);
            std::ifstream is(fOutName, std::ios::binary | std::ios::in);
        }
        double bSetup = timer.stop();


        std::cout << "Average setup time of ArrayStreamReader setup is " << streamSetup / (double)iterations << " seconds." << std::endl;
        std::cout << "Average setup time of ArrayFileReader setup is " << readerSetup / (double)iterations << " seconds." << std::endl;
        std::cout << "Average setup time of BinaryFileReader setup is " << bSetup / (double)iterations << " seconds." << std::endl;
    }

    /////////////////////////////////////////////////////////////////////


    if(testType == "RUN") {

        std::cout << "RUN:" << std::endl;

        /*
        std::cout << "ARRAY FILE STREAMER:" << std::endl;
        srand(181);
        std::ifstream myFile;
        myFile.open(fname);
        std::string line;
        size_t mNumberLines = 0;
        while (!myFile.eof()) {
            getline(myFile, line);
            mNumberLines++;
        }

        timer.start();

        for (size_t i = 0; i < iterations; i++) {
            if (verbose) std::cout << "iteration :" << i + 1 << std::endl;
            for (size_t j = 0; j < mNumberLines * asrHandicap; j++) {
                myFile.seekg(std::ios_base::beg);
                std::vector<double> row = getLineOfFile(myFile,
                                                        rand() % mNumberLines, mNumberLines, delimiter);
                //do something with data
                myFile.clear();
                if (verbose && j % 1000 == 0 && j != 0) {
                    std::cout << j << std::endl;
                }
            }
        }

        double streamTime = timer.stop() / asrHandicap;
        myFile.close();

        //////////////////////////////////////////////////

        srand(181);
        std::cout << "ARRAY FILE READER:" << std::endl;
        ArrayFileReader<double> arrayFileReader = ArrayFileReader<double>(fname, delimiter);
        timer.start();

        for (size_t i = 0; i < iterations; i++) {
            if (verbose) std::cout << "iteration :" << i + 1 << std::endl;
            for (size_t j = 0; j < arrayFileReader.getNumberOfLines() * afrHandicap; j++) {
                std::vector<double> row = arrayFileReader.getLine(rand()
                                                                  % arrayFileReader.getNumberOfLines());
                if (verbose && j % 10000 == 0 && j != 0) {
                    std::cout << j << std::endl;
                }
            }
        }

        double readTime = timer.stop() / afrHandicap;



        *//////////////////////////////////////////////////



        srand(181);
        std::cout << "Binary File Parser:" << std::endl;

        const char *fOutName = "myTestOut.txt";
        BinaryFileParser binaryFileParser = BinaryFileParser(fname, fOutName, delimiter);
        std::ifstream is(fOutName, std::ios::binary | std::ios::in);
        Timer timerSecond;
        double getlineTimeOutside = 0;
        double overHeadTime = 0;

        timer.start();
        timerSecond.start();
        for (size_t i = 0; i < iterations; i++) {
            if (verbose) std::cout << "iteration :" << i + 1 << std::endl;
            for (size_t j = 0; j < binaryFileParser.getNumberOfLines() * bfpHandicap; j++) {
                overHeadTime += timerSecond.stop();
                timerSecond.start();
                std::vector<double> row = binaryFileParser.getLine(rand()
                                                                   % binaryFileParser.getNumberOfLines(), is);
                getlineTimeOutside += timerSecond.stop();
                timerSecond.start();
                if (verbose && j % 10000 == 0 && j != 0) {
                    std::cout << j << std::endl;
                }
            }
        }
        overHeadTime += timerSecond.stop();

        double binaryTime = timer.stop();

        std::cout << "percent of time due to seeking lines: " << 100 * binaryFileParser.tSeekTime / binaryTime << std::endl;
        std::cout << "percent of time due to reading lines: " << 100 * binaryFileParser.tReadTime / binaryTime << std::endl;
        std::cout << "percent of time due to populating vector: " << 100 * binaryFileParser.tArrayTime / binaryTime << std::endl;
        std::cout << "percent Getline Total outside: " << 100 * getlineTimeOutside / binaryTime << std::endl;
        std::cout << "percent Overhead time: " << 100 * overHeadTime / binaryTime << std::endl;
        std::cout << "Getline Total outside: " << getlineTimeOutside << std::endl;
        std::cout << "Overhead time: " << overHeadTime << std::endl;
        std::cout << "Elapsed time: " << binaryTime << std::endl;


        std::cout << std::scientific;

                /*
        std::cout << "seconds per row read for ArrayStreamReader: " << streamTime /
                                                                       (iterations *
                                                                        (double) arrayFileReader.getNumberOfLines()) <<
        std::endl;


        std::cout << "seconds per row read for ArrayFileReader: " << readTime /
                                                                     (iterations *
                                                                      (double) arrayFileReader.getNumberOfLines()) <<
        std::endl;

        */
        std::cout << "seconds per row read for BinaryFileReader: " << binaryTime /
                                                                      (iterations *
                                                                       (double) binaryFileParser.getNumberOfLines()) <<
        std::endl;


        std::cout << "----------------------------------------------" << std::endl;
        //std::cout << "Elapsed time of ArrayStreamReader runtime is " << streamTime << " seconds." << std::endl;
        //std::cout << "Elapsed time of ArrayFileReader runtime is " << readTime << " seconds." << std::endl;
        std::cout << "Seconds per data pass of BinaryFileReader runtime is " << binaryTime / iterations
        << " seconds." << std::endl;


    }


    return 0;
}