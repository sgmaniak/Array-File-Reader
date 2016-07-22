//
// Created by anderson on 6/22/16.
//

#ifndef READ_FROM_DISK_SPARSEFILEPARSER_H
#define READ_FROM_DISK_SPARSEFILEPARSER_H

#include "array2dfileparser.h"
#include <string>

template <typename T>
class SArray2dFromFileOld {
private:

    std::string SPARSE_OUT_FILE_NAME = "out_sarray2dfileparser.txt";

    std::shared_ptr<Array2dFileParser<T>> _array2dfileparser;

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

    //returns the stream at the same position
    ulong find_num_columns(std::ifstream &ja_reader, long end_position) const {
        const long init_position = ja_reader.tellg();
        ulong max = 0;
        while(ja_reader.tellg() < end_position) {
            ulong cur = get_next_value_in_stream<ulong>(ja_reader);
            if(max < cur) max = cur;
        }
        ja_reader.seekg(init_position);
        return max + 1;
    }

    template <typename U>
    U parse_char_to_data_type(const std::vector<char> &c) const{
        size_t j = 0;
        U u;
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
            if (c == EOF || c == '\n' || c == 0) return 0;
        }while(is_delimiter(c));
        do {
            u.push_back(c);
            is.get(c);
        }while(!is_delimiter(c)
              && !(is.peek() == 10 || is.peek() == EOF || is.peek() == 0));
        return parse_char_to_data_type<U>(u);
    }

    void write_row_from_csr(std::ifstream &a_reader, std::ifstream &ja_reader, std::ofstream &os,
                            ulong num_values, ulong num_columns) const {
        ulong cur_column = 0;

        for (ulong i = 0; i < num_values; ++i) {
            T value = get_next_value_in_stream<T>(a_reader);
            ulong value_column = get_next_value_in_stream<ulong>(ja_reader);
            for (ulong j = 0; j < value_column - cur_column; ++j) {
                os << 0 << " ";
            }
            cur_column = value_column;
            os << value << " ";
            cur_column++;
        }

        // fill in 0's at end of row
        for (ulong i = 0; i < num_columns - cur_column; i++) {
            os << 0 << " ";
        }
        os << '\n';
    }

    const std::string& convert_from_csr(const std::string &file_name) const {
        //reads the A array of  values in matrix
        std::ifstream a_reader(file_name, std::ios::binary | std::ios::in);
        //reads the IA array which splits values of A into rows, 2nd line of file
        std::ifstream ja_reader(file_name, std::ios::binary | std::ios::in);
        ja_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
        //reads the JA array which splits values of A into columns, 3rd line of file
        std::ifstream ia_reader(file_name, std::ios::binary | std::ios::in);
        ia_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
        ia_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);

        std::ofstream os(SPARSE_OUT_FILE_NAME, std::ios::binary | std::ios::out);

        // get the number of rows in the matrix
        ulong num_rows = 0;
        if(get_next_value_in_stream<ulong>(ia_reader) != 0) throw std::invalid_argument("IA array must begin with 0");

        // position after initial 0 in IA array
        const long init_position = ia_reader.tellg();
        while(get_next_value_in_stream<ulong>(ia_reader) != 0) {
            num_rows++;
        }
        ia_reader.close();
        ia_reader.open(file_name, std::ios::binary | std::ios::in);
        ia_reader.seekg(init_position);

        ulong num_columns = find_num_columns(ja_reader, init_position);

        ulong total_num_values = 0;
        for (size_t i = 0; i < num_rows; ++i) {
            ulong cur_num_values = get_next_value_in_stream<ulong>(ia_reader) - total_num_values;
            write_row_from_csr(a_reader, ja_reader, os, cur_num_values, num_columns);
            total_num_values += cur_num_values;
        }

        return SPARSE_OUT_FILE_NAME;
    }

public:

    SArray2dFromFileOld(const std::string &file_name) {
        if(!file_exists(file_name)) throw std::invalid_argument("File not found");
        try {
            _array2dfileparser = std::make_shared<Array2dFileParser<T>>(convert_from_csr(file_name));
        }catch(std::invalid_argument &e){
            std::cout << "Array2dFileParser: " << e.what() << std::endl;
            throw e;
        }catch(std::range_error &re){
            std::cout << "Array2dFileParser: " << re.what() << std::endl;
            throw re;
        }catch(std::exception &e) {
            std::cout << "Array2dFileParser: " << e.what() << std::endl;
            throw e;
        }
    };

    ~SArray2dFromFileOld() {
        if( remove(SPARSE_OUT_FILE_NAME.c_str()) != 0 ) {
            std::cerr << "Error deleting file" << SPARSE_OUT_FILE_NAME.c_str() << std::endl;
        }
    }

    T* get_row(unsigned long row_num) const {return _array2dfileparser->get_row(row_num);};

    unsigned long get_num_rows() const {return _array2dfileparser->get_num_rows();}

    unsigned long get_num_columns() const {return _array2dfileparser->get_num_columns();};

};


#endif //READ_FROM_DISK_SPARSEFILEPARSER_H
