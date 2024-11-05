#include "api/utils/StringUtils.hpp"
#include <ctime>
#include <iomanip>
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

/*
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
*/

std::wstring stringtoWstring(std::string_view str, uint32_t codePage) {
    int          len = MultiByteToWideChar(codePage, 0, str.data(), (int)str.size(), nullptr, 0);
    std::wstring wstr;
    if (len == 0) return wstr;
    wstr.resize(len);
    MultiByteToWideChar(codePage, 0, str.data(), (int)str.size(), wstr.data(), len);
    return wstr;
}

std::string wstringtoString(std::wstring_view wstr, uint32_t codePage) {
    int         len = WideCharToMultiByte(codePage, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str;
    if (len == 0) return str;
    str.resize(len);
    WideCharToMultiByte(codePage, 0, wstr.data(), (int)wstr.size(), str.data(), (int)str.size(), nullptr, nullptr);
    return str;
}

std::string getTimeStringFormatted(std::string const& format) {
    time_t    time_s = time(0);
    struct tm now_tm;
    localtime_s(&now_tm, &time_s);
    std::stringstream ss;
    ss << std::put_time(&now_tm, format.c_str());
    return ss.str();
}

std::string trimString(std::string const& str) {
    if (str.empty()) {
        return str;
    }
    std::size_t first = 0;
    while (first < str.size() && std::isspace(static_cast<unsigned char>(str[first]))) {
        ++first;
    }
    std::size_t last = str.size() - 1;
    while (last > first && std::isspace(static_cast<unsigned char>(str[last]))) {
        --last;
    }
    if (first > last) {
        return {}; // 如果字符串全是空白字符，返回空字符串
    }
    return str.substr(first, (last - first + 1));
}

} // namespace utils
