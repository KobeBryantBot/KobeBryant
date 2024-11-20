#pragma once
#include "Macros.hpp"
#include <iostream>

namespace utils {

class UUID {
public:
    uint64_t mHigh;
    uint64_t mLow;

    KobeBryant_NDAPI UUID();
    KobeBryant_NDAPI UUID(uint64_t high, uint64_t low);

    KobeBryant_NDAPI static UUID random();

    KobeBryant_NDAPI static UUID fromString(const std::string& str);

    KobeBryant_NDAPI static UUID fromBinary(const std::string& str);

    KobeBryant_NDAPI std::string toString() const;

    KobeBryant_NDAPI std::string toBinary() const;

    KobeBryant_NDAPI bool isValid() const;

    KobeBryant_NDAPI bool operator==(const UUID&) const;

    static const UUID INVALID;
};

} // namespace utils

namespace std {
template <>
struct hash<utils::UUID> {
    KobeBryant_NDAPI size_t operator()(const utils::UUID&) const;
};
} // namespace std