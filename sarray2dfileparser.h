//
// Created by anderson on 6/22/16.
//

#ifndef READ_FROM_DISK_SPARSEFILEPARSER_H
#define READ_FROM_DISK_SPARSEFILEPARSER_H

#include "array2dfileparser.h"
#include <string>

template <typename T>
class SArray2dFileParser {
private:

    const std::string &OUT_FILE_NAME = "out_sarray2dfileparser.txt";

    std::unique_ptr<Array2dFileParser<T>> _array2dfileparser;

    bool is_delimiter(const char c) const{
        return (c < '0' || c > '9')
               && c != '.'
               && c != '-'
               && (int)c != 10
               && c != EOF;
    }

    inline bool file_exists(const std::string &file_name) const{
        struct stat buffer;
        return (stat (file_name.c_str(), &buffer) == 0);
    }

    template <typename U>
    U parse_char_to_data_type(const std::vector<char> &c) const{
        size_t j = 0;
        U u;
        std::cout << "parse! :)" << std::endl;
        //not exactly correct size but guarantees more than enough room for type T
        size_t buffer_size = 256;
        char buffer[buffer_size];
        size_t buffer_index = 0;
        while(is_delimiter(c[j])) j++;
        while(!is_delimiter(c[j]) && c[j] != 10 && c[j] != EOF) {
            buffer[buffer_index] = c[j];
            j++;
            buffer_index++;
        }
        //parse the data as the correct type
        if(std::is_same<U, float>::value)u = std::stof(buffer, nullptr);
        else if(std::is_same<U, int>::value)u = atoi(buffer);
        else if(std::is_same<U, unsigned int>::value) {
            unsigned long val = strtoul(buffer, nullptr, 10);
            if (val > std::numeric_limits<unsigned int>::max() || val < std::numeric_limits<unsigned int>::min()) {
                std::string error = "value out of range for type unsigned int: ";
                throw std::range_error(error + std::to_string(val));
            }
            u = (unsigned int)val;
        }
        else if(std::is_same<U, short>::value) {
            int val = atoi(buffer);
            if (val > std::numeric_limits<short>::max() || val < std::numeric_limits<short>::min()) {
                std::string error = "value out of range for type short: ";
                throw std::range_error(error + std::to_string(val));
            }
            u = (short)val;
        }
        else if(std::is_same<U, unsigned short>::value) {
            unsigned long val = strtoul(buffer, nullptr, 10);
            if (val > std::numeric_limits<unsigned short>::max() || val < std::numeric_limits<unsigned short>::min()) {
                std::string error = "value out of range for type unsigned short: ";
                throw std::range_error(error + std::to_string(val));
            }
            u = (unsigned short)val;
        }
        else if(std::is_same<U, long>::value)u = atol(buffer);
        else if(std::is_same<U, unsigned long>::value)u = strtoul(buffer, nullptr, 10);
        else if(std::is_same<U, double>::value)u = atof(buffer);

        return u;
    }

    template <typename U>
    const U get_next_value_in_stream(std::ifstream &is) const {
        char c = 0;
        std::vector<char> u;
        do {
            is.get(c);
        }while(is_delimiter(c) && !(c != '/n' || c != EOF));
        do {
            u.push_back(c);
            is.get(c);
        }while(!is_delimiter(c)
              && !(is.peek() == '/n' || is.peek() == EOF));
        return parse_char_to_data_type<U>(u);
    }

    void write_row_from_csr(std::ifstream &a_reader, std::ifstream &ja_reader, std::ofstream &os, ulong num_values) const {
        ulong cur_column = 0;
        for (ulong i = 0; i < num_values; ++i) {
            T value = get_next_value_in_stream<T>(a_reader);
            ulong value_column = get_next_value_in_stream<ulong>(ja_reader);
            for (ulong j = 0; j < value_column - cur_column; ++j) {
                os << 0 << " ";
            }
            cur_column = value_column;
            os << value << " ";
        }
        os << '\n';
    }

    const std::string&  convert_from_csr(const std::string &file_name) const {
        //reads the A array of  values in matrix
        std::ifstream a_reader(file_name, std::ios::binary | std::ios::in);
        //reads the IA array which splits values of A into rows, 2nd line of file
        std::ifstream ja_reader(file_name, std::ios::binary | std::ios::in);
        ja_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
        //reads the JA array which splits values of A into columns, 3rd line of file
        std::ifstream ia_reader(file_name, std::ios::binary | std::ios::in);
        ia_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
        ia_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);

        std::ofstream os(OUT_FILE_NAME, std::ios::binary | std::ios::out);

        // get the number of rows in the matrix
        long init_position = ia_reader.tellg();
        ulong num_rows = 0;
        if(get_next_value_in_stream<ulong>(ia_reader) != 0) throw std::invalid_argument("IA array must begin with 0");

        do {
            num_rows++;
        }while(get_next_value_in_stream<ulong>(ia_reader) != 0);
        ia_reader.seekg(init_position);

        ulong total_num_values = 0;
        for (size_t i = 0; i < num_rows; ++i) {
            ulong cur_num_values = get_next_value_in_stream<ulong>(ia_reader) - total_num_values;
            write_row_from_csr(a_reader, ja_reader, os, cur_num_values);
        }

        return OUT_FILE_NAME;
    }

public:

    SArray2dFileParser(const std::string &file_name) {
        if(!file_exists(file_name)) throw std::invalid_argument("File not found");
        *_array2dfileparser = std::move(Array2dFileParser<T>(convert_from_csr(file_name)));
    };

    ~SArray2dFileParser(){ }

    T* get_row(unsigned long row_num) const {return _array2dfileparser->get_row(row_num);};

    unsigned long get_num_rows() const {return _array2dfileparser->get_num_rows();}

    unsigned long get_num_columns() const {return _array2dfileparser->get_num_columns();};

};


#endif //READ_FROM_DISK_SPARSEFILEPARSER_H
