//
// Created by anderson on 7/22/16.
//

#include <cstdio>

template <typename T>
void clear_buffer(T buffer[], const size_t buffer_size) {
    for (size_t i = 0; i < buffer_size; i++) {
        buffer[i] = 0;
    }
}

inline bool file_exists(const std::string &file_name) {
    struct stat buffer;
    return (stat (file_name.c_str(), &buffer) == 0);
}

template <typename T>
T parse_buffer_to_datatype(char buffer[]) {
    if(std::is_same<T, float>::value) return std::stof(buffer, nullptr);
    else if(std::is_same<T, int>::value) return atoi(buffer);
    else if(std::is_same<T, unsigned int>::value) {
        unsigned long val = strtoul(buffer, nullptr, 10);
        if (val > std::numeric_limits<unsigned int>::max() || val < std::numeric_limits<unsigned int>::min()) {
            std::string error = "value out of range for type unsigned int: ";
            throw std::range_error(error + std::to_string(val));
        }
        return (unsigned int)val;
    }
    else if(std::is_same<T, short>::value) {
        int val = atoi(buffer);
        if (val > std::numeric_limits<short>::max() || val < std::numeric_limits<short>::min()) {
            std::string error = "value out of range for type short: ";
            throw std::range_error(error + std::to_string(val));
        }
        return (short)val;
    }
    else if(std::is_same<T, unsigned short>::value) {
        unsigned long val = strtoul(buffer, nullptr, 10);
        if (val > std::numeric_limits<unsigned short>::max() || val < std::numeric_limits<unsigned short>::min()) {
            std::string error = "value out of range for type unsigned short: ";
            throw std::range_error(error + std::to_string(val));
        }
        return (unsigned short)val;
    }
    else if(std::is_same<T, long>::value) return atol(buffer);
    else if(std::is_same<T, unsigned long>::value) return strtoul(buffer, nullptr, 10);
    else if(std::is_same<T, double>::value) return atof(buffer);
}

template <typename T>
void write_bytes(std::ofstream &os, T *t, ulong n_values) {
    os.write(reinterpret_cast<const char *>(t), std::streamsize(n_values * sizeof(T)));
}

template <typename T>
void read_bytes(std::ifstream &is, T *row, ulong n_values) {
    is.read(reinterpret_cast<char*>(row), std::streamsize(n_values * sizeof(T)));
}