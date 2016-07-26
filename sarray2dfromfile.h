//
// Created by anderson on 7/22/16.
//

#ifndef READ_FROM_DISK_SARRAY2DFROMFILE_H
#define READ_FROM_DISK_SARRAY2DFROMFILE_H

#include <stdexcept>
#include <ios>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>
#include <limits>
#include "utilsarray2dfromfile.h"

template <typename T>
class SArray2dFromFile {
private:

    std::string OUT_FILE_NAME = "out-sarray2dfromfile.txt";
    std::string INDEX_OUT_FILE_NAME = "index-sarray2dfromfile.txt";
    ulong OUT_FILE_SIZE = 10000;

    ulong _n_rows;
    ulong _n_cols;
    std::vector<std::string> _data_file_list;
    std::vector<std::string> _index_file_list;
    std::vector<long> _file_position_index;
    std::vector<ulong> _nvalues_index;

    void clear_output_files();

    bool is_delimiter(const char c) const;

    std::string find_available_file_name(ulong &cur_file_index, const std::string &base_name);

    ulong find_num_columns(std::ifstream &ja_reader, long end_position) const;

    template <typename U>
    const U get_next_value_in_stream(std::ifstream &is) const;

    void parse(const std::string &file_name);

public:

    SArray2dFromFile(const std::string &file_name);

    ~SArray2dFromFile();

    T* get_row(unsigned long row_num);

    unsigned long get_num_non_zero_in_row(ulong row_num) const {return _nvalues_index[row_num];};

    unsigned long get_num_cols() const {return _n_cols;};

    unsigned long get_num_rows() const {return _n_rows;};

};


#endif //READ_FROM_DISK_SARRAY2DFROMFILE_H
