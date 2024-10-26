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

std::wstring toWstring(const std::string& utf8) {
    // 获取所需的缓冲区大小
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    // 分配缓冲区并进行转换
    std::wstring utf16(len, L'\0');
    len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], len);
    // 去除末尾的空字符
    utf16.resize(len - 1);
    return utf16;
}

} // namespace utils
