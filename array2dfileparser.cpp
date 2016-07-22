//
// Created by Matthew Anderson 6/13/16

#include <bits/unique_ptr.h>
#include "array2dfileparser.h"

template <typename T>
template <typename U>
void Array2dFileParser<T>::clear_buffer(U buffer[], const size_t buffer_size) const {
    for (size_t i = 0; i < buffer_size; i++) {
        buffer[i] = 0;
    }
}

template <typename T>
void Array2dFileParser<T>::clear_output_files() {
    for(size_t i = 0; i < _file_list.size(); i++) {
        if( remove(_file_list.back().c_str()) != 0 ) {
            std::cerr << "Error deleting file" << _file_list.back() << std::endl;
        }
        _file_list.pop_back();
    }
}

/*
 * checks if char c is a delimiting character
 * delimiting characters are any characters that are not the following:
 * in the range of 0 - 9
 * a negative sign -
 * a period .
 * a newline \n
 * an end of file EOF
 */
template <typename T>
bool Array2dFileParser<T>::is_delimiter(const char c) const{
    return (c < '0' || c > '9')
           && c != '.'
           && c != '-'
           && (int)c != 10
           && c != EOF;
}

template <typename T>
inline bool Array2dFileParser<T>::file_exists(const std::string &file_name) const{
    struct stat buffer;
    return (stat (file_name.c_str(), &buffer) == 0);
}

template <typename T>
unsigned long Array2dFileParser<T>::find_num_columns(const std::string &file_name, unsigned long row) const{
    unsigned long n_columns = 1;
    char c;
    std::ifstream is(file_name, std::ios::binary | std::ios::in);
    std::string junk_line;
    for (size_t i = 0; i < row; ++i) {
        std::getline(is, junk_line);
    }
    do {
        is.get(c);
        if(c == 0) return 0;
        if (is_delimiter(c)
            && !is_delimiter((char)is.peek())
            && !(is.peek() == 10 || is.peek() == EOF)) n_columns++;
    } while ((int) c != 10 && c != EOF);
    return n_columns;
}

template <typename T>
std::unique_ptr<T[]> Array2dFileParser<T>::parse_char_to_data_type(const std::vector<char> &c) const{
    std::unique_ptr<T[]> t(new T[_n_cols_parser]);
    size_t j = 0;

    //not exactly correct size but guarantees more than enough room for type T
    size_t buffer_size = 256;
    char buffer[buffer_size];
    for (size_t i = 0; i < _n_cols_parser; i++) {
        size_t buffer_index = 0;
        while(is_delimiter(c[j])) j++;
        while(!is_delimiter(c[j]) && c[j] != 10 && c[j] != EOF) {
            buffer[buffer_index] = c[j];
            j++;
            buffer_index++;
        }
        //parse the data as the correct type
        if(std::is_same<T, float>::value)t[i] = std::stof(buffer, nullptr);
        if(std::is_same<T, int>::value)t[i] = atoi(buffer);
        if(std::is_same<T, unsigned int>::value) {
            unsigned long val = strtoul(buffer, nullptr, 10);
            if (val > std::numeric_limits<unsigned int>::max() || val < std::numeric_limits<unsigned int>::min()) {
                std::string error = "value out of range for type unsigned int: ";
                throw std::range_error(error + std::to_string(val));
            }
            t[i] = (unsigned int)val;
        }
        if(std::is_same<T, short>::value) {
            int val = atoi(buffer);
            if (val > std::numeric_limits<short>::max() || val < std::numeric_limits<short>::min()) {
                std::string error = "value out of range for type short: ";
                throw std::range_error(error + std::to_string(val));
            }
            t[i] = (short)val;
        }
        if(std::is_same<T, unsigned short>::value) {
            unsigned long val = strtoul(buffer, nullptr, 10);
            if (val > std::numeric_limits<unsigned short>::max() || val < std::numeric_limits<unsigned short>::min()) {
                std::string error = "value out of range for type unsigned short: ";
                throw std::range_error(error + std::to_string(val));
            }
            t[i] = (unsigned short)val;
        }
        if(std::is_same<T, long>::value)t[i] = atol(buffer);
        if(std::is_same<T, unsigned long>::value)t[i] = strtoul(buffer, nullptr, 10);
        if(std::is_same<T, double>::value)t[i] = atof(buffer);
        j++;
        clear_buffer<char>(buffer, buffer_size);
    }
    return t;
}

template <typename T>
void Array2dFileParser<T>::parse(const std::string &file_name) {

    std::ifstream is(file_name, std::ios::binary | std::ios::in);

    // find available file name to avoid overwriting existing files potentially
    // owned by other fileparser objects
    std::string cur_outfile_name;
    ulong cur_file_index = 0;
    do {
        std::ostringstream oss;
        oss << cur_file_index << "-";
        cur_outfile_name = oss.str() + OUT_FILE_NAME;
        cur_file_index++;
    } while(file_exists(cur_outfile_name.c_str()));

    std::ofstream os(cur_outfile_name, std::ios::binary | std::ios::out);
    _file_list.push_back(cur_outfile_name);
    std::vector<char> row;
    char c;
    ulong line_size = 1;

    try {
        do {
            c = (char) is.get();
            row.push_back(c);
            if (is_delimiter(c)
                && !is_delimiter((char) is.peek())
                && !(is.peek() == 10 || is.peek() == EOF))
                line_size++;

            if ((int) c == 10 || c == EOF && row[0] != EOF) {
                if (line_size != _n_cols_parser) throw std::invalid_argument("File does not contain dense matrix.");
                std::unique_ptr<T[]> t = parse_char_to_data_type(row);
                os.write(reinterpret_cast<const char *>(t.get()),
                         std::streamsize(_n_cols_parser * sizeof(T)));
                _n_rows_parser++;
                row.clear();
                if (_n_rows_parser % OUT_FILE_SIZE == 0) {
                    os.close();
                    do {
                        std::ostringstream oss;
                        oss << cur_file_index << "-";
                        cur_outfile_name = oss.str() + OUT_FILE_NAME;
                        cur_file_index++;
                    } while (file_exists(cur_outfile_name.c_str()));
                    os.open(cur_outfile_name, std::ios::binary | std::ios::out);
                    _file_list.push_back(cur_outfile_name);
                }
                line_size = 1;
            }
        } while (c != EOF);
    }catch(std::invalid_argument &e) {
        if(is.is_open()) is.close();
        if(os.is_open()) os.close();
        clear_output_files();
        throw e;
    }catch(std::range_error &re) {
        if(is.is_open()) is.close();
        if(os.is_open()) os.close();
        clear_output_files();
        throw re;
    }
}

template <typename T>
Array2dFileParser<T>::Array2dFileParser(const std::string &file_name){
    std::cout << "entered Array2dFileParser constructor" << std::endl;
    if(!file_exists(file_name)) throw std::invalid_argument("File not found");
    _n_rows_parser = 0;
    _n_cols_parser = find_num_columns(file_name, 0);
    if(_n_cols_parser == 0) throw std::invalid_argument("File is empty");
    parse(file_name);
}

template <typename T>
Array2dFileParser<T>::Array2dFileParser(Array2dFileParser &&rh){
    *this = std::move(rh);
}

template <typename T>
Array2dFileParser<T>& Array2dFileParser<T>::operator=(Array2dFileParser<T> &&rh){
    if(this == &rh) return *this;
    _n_cols_parser = rh._n_cols_parser;
    _n_rows_parser = rh._n_rows_parser;
    _file_list = std::move(rh._file_list);
    return *this;
}

template <typename T>
Array2dFileParser<T>::~Array2dFileParser() {
    clear_output_files();
}

template <typename T>
T* Array2dFileParser<T>::get_row(unsigned long row_num) const{
    if (row_num >= _n_rows_parser) {
        throw std::invalid_argument("number out of range" + row_num);
    }
    std::ifstream is(_file_list[row_num / OUT_FILE_SIZE], std::ios::binary | std::ios::in);
    is.seekg(_n_cols_parser * row_num * sizeof(T) % (_n_cols_parser * OUT_FILE_SIZE * sizeof(T)));
    T *row = new T[_n_cols_parser];
    is.read(reinterpret_cast<char*>(row), std::streamsize(_n_cols_parser * sizeof(T)));
    return row;
}


template class Array2dFileParser<float>;
template class Array2dFileParser<int>;
template class Array2dFileParser<unsigned int>;
template class Array2dFileParser<short>;
template class Array2dFileParser<unsigned short>;
template class Array2dFileParser<long>;
template class Array2dFileParser<unsigned long>;
template class Array2dFileParser<double>;
