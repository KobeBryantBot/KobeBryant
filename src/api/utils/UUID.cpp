#include "api/utils/UUID.hpp"
#include "core/modules/KobeBryant.hpp"
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

UUID::UUID() : mHigh(0), mLow(0) {}

UUID::UUID(uint64_t a, uint64_t b) : mHigh(a), mLow(b) {}

UUID UUID::random() {
    UUID                                    uuid;
    std::random_device                      rd;
    std::mt19937_64                         eng(rd());
    std::uniform_int_distribution<uint64_t> distr;
    uuid.mHigh = distr(eng);
    uuid.mLow  = distr(eng);
    return uuid;
}

std::string UUID::toString() const {
    std::string uuid      = "";
    std::string part1_hex = ulltohex(mHigh);
    std::string part2_hex = ulltohex(mLow);
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
    try {
        if (str.length() != 36 || str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
            return UUID::INVALID;
        }
        std::string part1 = str.substr(0, 8);
        std::string part2 = str.substr(9, 4);
        std::string part3 = str.substr(14, 4);
        std::string part4 = str.substr(19, 4);
        std::string part5 = str.substr(24);
        result.mHigh      = std::stoull(part1, nullptr, 16) << 32 | std::stoull(part2 + part3, nullptr, 16);
        result.mLow       = std::stoull(part4 + part5, nullptr, 16);
    }
    CATCH
    return result;
}

bool UUID::isValid() const {
    return mHigh != std::numeric_limits<uint64_t>::max() || mLow != std::numeric_limits<uint64_t>::max();
}

bool UUID::operator==(const UUID& rhs) const { return (mHigh == rhs.mHigh) && (mLow == rhs.mLow); }

UUID UUID::fromBinary(std::string const& str) {
    auto first  = *reinterpret_cast<const uint64_t*>(str.data());
    auto second = *reinterpret_cast<const uint64_t*>(str.data() + 8);
    return UUID(first, second);
}

std::string UUID::toBinary() const {
    std::string result(16, '\0');
    memcpy(result.data(), &mHigh, 8);
    memcpy(result.data() + 8, &mLow, 8);
    return result;
}

const UUID UUID::INVALID = UUID{std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()};

} // namespace utils

size_t std::hash<utils::UUID>::operator()(const utils::UUID& obj) const {
    std::size_t hash1  = std::hash<uint64_t>{}(obj.mHigh);
    std::size_t hash2  = std::hash<uint64_t>{}(obj.mLow);
    hash1             ^= hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
    hash2             ^= hash1 + 0x9e3779b9 + (hash2 << 6) + (hash2 >> 2);
    return hash1 ^ hash2;
}