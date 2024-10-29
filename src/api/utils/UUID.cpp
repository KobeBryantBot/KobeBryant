#include "api/utils/UUID.hpp"
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace utils {

std::string ulltohex(uint64_t num) {
    std::stringstream ss;
    ss << std::hex << num;
    std::string hex = ss.str();
    while (hex.size() < 16) {
        hex = "0" + hex;
    }
    return hex;
}

UUID::UUID() : mFirst(0), mSecond(0) {}

UUID::UUID(uint64_t a, uint64_t b) : mFirst(a), mSecond(b) {}

UUID UUID::random() {
    UUID                                    uuid;
    std::random_device                      rd;
    std::mt19937_64                         eng(rd());
    std::uniform_int_distribution<uint64_t> distr;
    uuid.mFirst  = distr(eng);
    uuid.mSecond = distr(eng);
    return uuid;
}

std::string UUID::toString() const {
    std::string uuid      = "";
    std::string part1_hex = ulltohex(mFirst);
    std::string part2_hex = ulltohex(mSecond);
    // UUID format: 8-4-4-4-12
    uuid += part1_hex.substr(0, 8); // first 8 characters
    uuid += "-";
    uuid += part1_hex.substr(8, 4); // next 4 characters
    uuid += "-";
    uuid += part1_hex.substr(12, 4); // next 4 characters
    uuid += "-";
    uuid += part2_hex.substr(0, 4); // next 4 characters
    uuid += "-";
    uuid += part2_hex.substr(4, 12); // last 12 characters
    return uuid;
}

UUID UUID::fromString(std::string const& str) {
    auto result = UUID();
    if (str.length() != 36 || str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
        return UUID::INVALID;
    }
    std::istringstream ss(str.substr(0, 8), std::istringstream::hex);
    unsigned int       highPart;
    ss >> highPart;
    result.mFirst = static_cast<uint64_t>(highPart) << 32;

    ss.clear();
    ss.str(std::string(str, 9, 4));
    unsigned int lowPart;
    ss >> lowPart;
    result.mFirst |= lowPart;

    ss.clear();
    ss.str(std::string(str, 14, 4));
    ss >> highPart;
    result.mSecond = static_cast<uint64_t>(highPart) << 48;

    ss.clear();
    ss.str(std::string(str, 19, 4));
    ss >> lowPart;
    result.mSecond |= static_cast<uint64_t>(lowPart) << 16;

    ss.clear();
    ss.str(std::string(str, 24, 12));
    ss >> lowPart;
    result.mSecond |= lowPart;

    return result;
}

bool UUID::isValid() const {
    return mFirst != std::numeric_limits<uint64_t>::max() || mSecond != std::numeric_limits<uint64_t>::max();
}

bool UUID::operator==(const UUID& rhs) const { return (mFirst == rhs.mFirst) && (mSecond == rhs.mSecond); }

UUID UUID::fromBinary(std::string const& str) {
    auto first  = *reinterpret_cast<const uint64_t*>(str.data());
    auto second = *reinterpret_cast<const uint64_t*>(str.data() + 8);
    return UUID(first, second);
}

std::string UUID::toBinary() const {
    std::string result(16, '\0');
    memcpy(result.data(), &mFirst, 8);
    memcpy(result.data() + 8, &mSecond, 8);
    return result;
}

const UUID UUID::INVALID = UUID{std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()};

} // namespace utils

size_t std::hash<utils::UUID>::operator()(const utils::UUID& obj) const {
    std::size_t hash1  = std::hash<uint64_t>{}(obj.mFirst);
    std::size_t hash2  = std::hash<uint64_t>{}(obj.mSecond);
    hash1             ^= hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
    hash2             ^= hash1 + 0x9e3779b9 + (hash2 << 6) + (hash2 >> 2);
    return hash1 ^ hash2;
}