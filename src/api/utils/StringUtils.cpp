#include "api/utils/StringUtils.hpp"
#include <iostream>
#include <sstream>
#include <vector>

namespace utils {

std::string toCamelCase(std::string_view str) {
    std::stringstream result;
    bool              capitalizeNext = true;
    for (char c : std::string(str)) {
        if (c == '_' || c == ' ') {
            capitalizeNext = true;
        } else {
            if (capitalizeNext) {
                result << static_cast<char>(toupper(c));
                capitalizeNext = false;
            } else {
                result << c;
            }
        }
    }
    return result.str();
}

std::string toSnakeCase(std::string_view str) {
    std::string res;
    if (str.empty()) {
        return res;
    }
    for (size_t i = 0; i < str.size(); ++i) {
        if (isupper(str[i])) {
            if (i > 0 && ((i + 1 < str.size() && !isupper(str[i + 1])) || !isupper(str[i - 1]))) {
                res.push_back('_');
            }
            res.push_back((char)tolower(str[i]));
        } else {
            res.push_back(str[i]);
        }
    }
    return res;
}

std::wstring toWstring(std::string const& str) {
    if (str.empty()) return std::wstring();
    int                  size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::vector<wchar_t> buf(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &buf[0], size_needed);
    return std::wstring(buf.begin(), buf.end() - 1);
}

} // namespace utils
