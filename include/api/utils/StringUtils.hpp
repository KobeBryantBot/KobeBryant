#pragma once
#include "Macros.hpp"
#include <iostream>
#include <windows.h>

namespace utils {

constexpr std::string& ReplaceStr(std::string& str, std::string_view oldValue, std::string_view newValue) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += newValue.length()) {
        if ((pos = str.find(oldValue, pos)) != std::string::npos) str.replace(pos, oldValue.length(), newValue);
        else break;
    }
    return str;
}

[[nodiscard]] constexpr uint64_t doHash(std::string_view x) {
    uint64_t           hash  = 0xcbf29ce484222325;
    constexpr uint64_t prime = 0x100000001b3;
    for (char c : x) {
        hash ^= c;
        hash *= prime;
    }
    return hash;
}

KobeBryant_NDAPI std::string toCamelCase(std::string_view str);

KobeBryant_NDAPI std::string toSnakeCase(std::string_view str);

KobeBryant_NDAPI std::wstring toWstring(std::string const& str);

KobeBryant_NDAPI std::string getTimeStringFormatted(std::string const& format = "%Y-%m-%d %H:%M:%S");

} // namespace utils
