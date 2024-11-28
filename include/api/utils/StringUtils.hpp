#pragma once
#include "Macros.hpp"
#include <iostream>
#include <vector>

namespace utils {

constexpr inline std::string& ReplaceStr(std::string& str, std::string_view oldValue, std::string_view newValue) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += newValue.length()) {
        if ((pos = str.find(oldValue, pos)) != std::string::npos) str.replace(pos, oldValue.length(), newValue);
        else break;
    }
    return str;
}

[[nodiscard]] constexpr inline std::string
ReplaceStr(const std::string& str, std::string_view oldValue, std::string_view newValue) {
    std::string ret = str;
    ReplaceStr(ret, oldValue, newValue);
    return ret;
}

[[nodiscard]] constexpr inline uint64_t doHash(std::string_view x) {
    uint64_t           hash  = 0xcbf29ce484222325;
    constexpr uint64_t prime = 0x100000001b3;
    for (char c : x) {
        hash ^= c;
        hash *= prime;
    }
    return hash;
}

constexpr inline bool
replaceContent(std::string& str, std::string_view before, std::string_view after, std::string_view relplaceWith) {
    auto startOffset = str.find(before);
    if (startOffset == std::string::npos) return false;
    startOffset    += before.size();
    auto endOffset  = after.empty() ? std::string::npos : str.find(after, startOffset);
    str.replace(startOffset, endOffset - startOffset, relplaceWith);
    return true;
}

constexpr inline std::vector<std::string>
splitByPattern(std::string_view s, std::string_view pattern, bool keepEmpty = false) {
    std::vector<std::string> result;
    if (s.empty()) {
        return result;
    }
    size_t pos  = s.find(pattern);
    size_t size = s.size();
    while (pos != std::string::npos) {
        if (keepEmpty || pos != 0) {
            result.push_back(std::string(s.substr(0, pos)));
        }
        s   = s.substr(pos + pattern.size(), size - pos - pattern.size());
        pos = s.find(pattern);
    }
    if (keepEmpty || !s.empty()) {
        result.push_back(std::string(s));
    }
    return result;
}

KobeBryant_NDAPI std::string toCamelCase(std::string_view str);

KobeBryant_NDAPI std::string toSnakeCase(std::string_view str);

#ifdef _WIN32

namespace CodePage {
enum : uint32_t {
    DefaultACP = 0,  // default to ANSI code page
    ThreadACP  = 3,  // current thread's ANSI code page
    Symbol     = 42, // SYMBOL translations
    GB2312     = 936,
    UTF8       = 65001,
};
} // namespace CodePage

KobeBryant_NDAPI std::wstring stringtoWstring(std::string_view str, uint32_t codePage = CodePage::UTF8);

KobeBryant_NDAPI std::string wstringtoString(std::wstring_view wstr, uint32_t codePage = CodePage::UTF8);

#else

#endif

KobeBryant_NDAPI std::string getTimeStringFormatted(const std::string& format = "%Y-%M-%D %H:%M:%S");

KobeBryant_NDAPI std::string trimString(const std::string& str);

KobeBryant_NDAPI std::string toBinaryString(const std::vector<uint8_t>& binary);

KobeBryant_NDAPI std::vector<uint8_t> toBinaryArray(const std::string& binary);

} // namespace utils
