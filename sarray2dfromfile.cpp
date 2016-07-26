//
// Created by anderson on 7/22/16.
//

#include "sarray2dfromfile.h"

template <typename T>
void SArray2dFromFile<T>::clear_output_files() {
    for(size_t i = 0; i < _data_file_list.size(); i++) {
        if( remove(_data_file_list.back().c_str()) != 0 ) {
            std::cerr << "Error deleting file" << _data_file_list.back() << std::endl;
        }
        _data_file_list.pop_back();
    }
    for(size_t i = 0; i < _index_file_list.size(); i++) {
        if( remove(_index_file_list.back().c_str()) != 0 ) {
            std::cerr << "Error deleting file" << _index_file_list.back() << std::endl;
        }
        _index_file_list.pop_back();
    }
}

template <typename T>
bool SArray2dFromFile<T>::is_delimiter(const char c) const {
    return (c < '0' || c > '9')
           && c != '.'
           && c != '-'
           && (int)c != 10
           && c != EOF;
}

template <typename T>
std::string SArray2dFromFile<T>::find_available_file_name(ulong &cur_file_index, const std::string &base_name) {
    std::string cur_outfile_name;
    do {
        std::ostringstream oss;
        oss << cur_file_index << "-";
        cur_outfile_name = oss.str() + base_name;
        cur_file_index++;
    } while(file_exists(cur_outfile_name.c_str()));
    return cur_outfile_name;
}

//returns the stream at the same position
template <typename T>
ulong SArray2dFromFile<T>::find_num_columns(std::ifstream &ja_reader, long end_position) const {
    const long init_position = ja_reader.tellg();
    ulong max = 0;
    while(ja_reader.tellg() < end_position) {
        ulong cur = get_next_value_in_stream<ulong>(ja_reader);
        if(max < cur) max = cur;
    }
    ja_reader.seekg(init_position);
    return max + 1;
}

template <typename T>
template <typename U>
const U SArray2dFromFile<T>::get_next_value_in_stream(std::ifstream &is) const {
    char c = 0;
    size_t buffer_size = 256;
    char buffer[buffer_size];
    clear_buffer(buffer, buffer_size);
    do {
        is.get(c);
        if (c == EOF || c == '\n' || c == 0) return 0;
    }while(is_delimiter(c));
    size_t i = 0;
    do {
        buffer[i] = c;
        is.get(c);
        i++;
    }while(!is_delimiter(c)
           && !(c == 10 || c == EOF || c == 0)
           && i < buffer_size
           && is.good());
    return parse_buffer_to_datatype<U>(buffer);
}


template <typename T>
void SArray2dFromFile<T>::parse(const std::string &file_name) {

    //reads the A array of  values in matrix
    std::ifstream a_reader(file_name, std::ios::binary | std::ios::in);
    //reads the IA array which splits values of A into rows, 2nd line of file
    std::ifstream ja_reader(file_name, std::ios::binary | std::ios::in);
    ja_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
    //reads the JA array which splits values of A into columns, 3rd line of file
    std::ifstream ia_reader(file_name, std::ios::binary | std::ios::in);
    ia_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
    ia_reader.ignore(std::numeric_limits<std::streamsize>::max(), 10);
    if(get_next_value_in_stream<ulong>(ia_reader) != 0)
        throw std::invalid_argument("IA array must begin with 0 by definition");

    _n_cols = find_num_columns(ja_reader, ia_reader.tellg());

    ulong cur_data_file_index = 0;
    ulong cur_index_file_index = 0;

    // find available file name to avoid overwriting existing files potentially
    // owned by other fileparser objects
    std::string cur_outfile_name = find_available_file_name(cur_data_file_index, OUT_FILE_NAME);
    std::ofstream data_file(cur_outfile_name, std::ios::binary | std::ios::out);
    _data_file_list.push_back(cur_outfile_name);

    cur_outfile_name = find_available_file_name(cur_index_file_index, INDEX_OUT_FILE_NAME);
    std::ofstream index_file(cur_outfile_name, std::ios::binary | std::ios::out);
    _index_file_list.push_back(cur_outfile_name);

    try {
        ulong total_vals = 0;
        ulong n_vals = get_next_value_in_stream<ulong>(ia_reader);
        do {
            n_vals -= total_vals;
            total_vals += n_vals;
            _file_position_index.push_back((long)data_file.tellp());

            std::vector<T> t;
            std::vector<ulong> t_cols;
            for(ulong i = 0; i < n_vals; i++) {
                t.push_back(get_next_value_in_stream<T>(a_reader));
                t_cols.push_back(get_next_value_in_stream<ulong>(ja_reader));
            }
            _nvalues_index.push_back(n_vals);
            T *t_ptr = &t[0];
            ulong *t_col_ptr = &t_cols[0];
            write_bytes(data_file, t_ptr, n_vals);
            write_bytes(index_file, t_col_ptr, n_vals);
            _n_rows++;

            if (_n_rows % OUT_FILE_SIZE == 0) {
                data_file.close();
                cur_outfile_name = find_available_file_name(cur_data_file_index, OUT_FILE_NAME);
                data_file.open(cur_outfile_name, std::ios::binary | std::ios::out);
                _data_file_list.push_back(cur_outfile_name);

                index_file.close();
                cur_outfile_name = find_available_file_name(cur_index_file_index, OUT_FILE_NAME);
                index_file.open(cur_outfile_name, std::ios::binary | std::ios::out);
                _index_file_list.push_back(cur_outfile_name);
            }
            n_vals = get_next_value_in_stream<ulong>(ia_reader);
        } while (n_vals != 0);

    }catch(std::invalid_argument &e) {
        if(a_reader.is_open()) a_reader.close();
        if(ja_reader.is_open()) ja_reader.close();
        if(ia_reader.is_open()) ia_reader.close();
        if(data_file.is_open()) data_file.close();
        clear_output_files();
        throw e;
    }catch(std::range_error &re) {
        if(a_reader.is_open()) a_reader.close();
        if(ja_reader.is_open()) ja_reader.close();
        if(ia_reader.is_open()) ia_reader.close();
        if(data_file.is_open()) data_file.close();
        clear_output_files();
        throw re;
    }
}

template <typename T>
SArray2dFromFile<T>::SArray2dFromFile(const std::string &file_name){
    if(!file_exists(file_name)) throw std::invalid_argument("File does not exist");
    _n_rows = 0;
    _n_cols = 0;
    parse(file_name);
}

template <typename T>
SArray2dFromFile<T>::~SArray2dFromFile() {
    clear_output_files();
}

template <typename T>
T* SArray2dFromFile<T>::get_row(unsigned long row_num) {
    if(row_num >= _n_rows) throw std::invalid_argument("Invalid row_num " + row_num);
    std::ifstream data_reader(_data_file_list.at(row_num / OUT_FILE_SIZE), std::ios::binary | std::ios::in);
    std::ifstream index_reader(_index_file_list.at(row_num / OUT_FILE_SIZE), std::ios::binary | std::ios::in);
    data_reader.seekg(_file_position_index[row_num]);
    index_reader.seekg(_file_position_index[row_num]);
    ulong num_vals = _nvalues_index[row_num];

    std::unique_ptr<T[]> t_vals(new T[num_vals]);
    read_bytes(data_reader, t_vals.get(), num_vals);

    std::unique_ptr<ulong[]> t_cols(new ulong[num_vals]);
    read_bytes(index_reader, t_cols.get(), num_vals);

    T* row = new T[_n_cols];
    for (ulong i = 0; i < _n_cols; ++i) {
        row[i] = 0;
    }
    for (ulong i = 0; i < num_vals; ++i) {
        row[t_cols[i]] = t_vals[i];
    }
    return row;
}

template class SArray2dFromFile<float>;
template class SArray2dFromFile<int>;
template class SArray2dFromFile<unsigned int>;
template class SArray2dFromFile<short>;
template class SArray2dFromFile<unsigned short>;
template class SArray2dFromFile<long>;
template class SArray2dFromFile<unsigned long>;
template class SArray2dFromFile<double>;