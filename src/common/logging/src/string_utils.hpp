#pragma once

#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>

namespace utils {

inline std::string operator*(const std::string& word, uint32_t times) {
    std::string result;
    result.reserve(times * word.length());
    for (auto a = 0u; a < times; ++a) result += word;
    return result;
}

template<template<class...> class Container, class... Args>
inline std::string stringJoin(Container<std::string, Args...> sc,
                              std::string_view separator) {
    switch (sc.size()) {
    case 0: return std::string();
    case 1: return sc[0];
    default:
        std::ostringstream os;
        std::copy(sc.begin(), sc.end() - 1,
                  std::ostream_iterator<std::string>(os, separator.data()));
        os << *sc.rbegin();
        return os.str();
    }
}

}   // namespace utils
