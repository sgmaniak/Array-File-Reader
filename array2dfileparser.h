//
// Created by Matthew Anderson 6/13/16

#ifndef _ARRAY2DFILEPARSER_H
#define _ARRAY2DFILEPARSER_H

#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <vector>

template<typename T>
class Array2dFileParser {


private:

    const size_t OUT_FILE_SIZE = 10000;

    const std::string &OUT_FILE_NAME = "out_array2dfileparser.txt";

    unsigned long _n_rows_parser;
    unsigned long _n_cols_parser;
    std::vector<std::string> _file_list;

    template <typename U>
    void clear_buffer(U buffer[], const size_t buffer_size) const;

    void clear_output_files();

    bool is_delimiter(const char c) const;

    inline bool file_exists(const std::string &file_name) const;

    unsigned long find_num_columns(const std::string &file_name, unsigned long row) const;

    std::unique_ptr<T[]> parse_char_to_data_type(const std::vector<char> &c) const;

    void parse(const std::string &file_name);

    Array2dFileParser(const Array2dFileParser &rh) = delete;

    Array2dFileParser& operator=(const Array2dFileParser &rh) = delete;

public:


    //***********
    //NOTE: as of now, valid file format has delimiting character at the end of each row
    //**********

    Array2dFileParser(const std::string &file_name);

    Array2dFileParser(Array2dFileParser &&rh);

    Array2dFileParser& operator=(Array2dFileParser &&rh);

    ~Array2dFileParser();

    T* get_row(unsigned long row_num) const;

    unsigned long get_num_rows() const {return _n_rows_parser;}

    unsigned long get_num_columns() const {return _n_cols_parser;}

};

#endif