#pragma once
#include <sstream>
#include <string>
#include <stdexcept>


// don't worry too much it's chat gpt implementation of std::format
template<typename T>
void append_arg(std::ostringstream& oss, const T& arg) {
    oss << arg;
}

inline void format_recursive(std::ostringstream& oss, const std::string& fmt, size_t& i) {
    while (i < fmt.size()) {
        if (fmt[i] == '{' && i + 1 < fmt.size() && fmt[i + 1] == '}') {
            throw std::runtime_error("Too few arguments for format string");
        } else {
            oss << fmt[i++];
        }
    }
}

template<typename T, typename... Args>
void format_recursive(std::ostringstream& oss, const std::string& fmt, size_t& i, const T& first, const Args&... rest) {
    while (i < fmt.size()) {
        if (fmt[i] == '{' && i + 1 < fmt.size() && fmt[i + 1] == '}') {
            append_arg(oss, first);
            i += 2;
            format_recursive(oss, fmt, i, rest...);
            return;
        } else {
            oss << fmt[i++];
        }
    }
    throw std::runtime_error("Too many arguments for format string");
}

template<typename... Args>
std::string f(const std::string& fmt, const Args&... args) {
    std::ostringstream oss;
    size_t i = 0;
    format_recursive(oss, fmt, i, args...);
    return oss.str();
}

