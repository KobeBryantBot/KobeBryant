#pragma once
#include "Macros.hpp"
#include <iostream>

namespace utils {

class UUID {
public:
    uint64_t mFirst;
    uint64_t mSecond;

    KobeBryant_NDAPI UUID();
    KobeBryant_NDAPI UUID(uint64_t first, uint64_t second);

    KobeBryant_NDAPI static UUID random();

    KobeBryant_NDAPI static UUID fromString(std::string const& str);

    KobeBryant_NDAPI static UUID fromBinary(std::string const& str);

    KobeBryant_NDAPI std::string toString() const;

    KobeBryant_NDAPI std::string toBinary() const;

    KobeBryant_NDAPI bool isValid() const;

    KobeBryant_NDAPI bool operator==(const UUID& rhs) const;

    static const UUID INVALID;
};

} // namespace utils

namespace std {
template <>
struct hash<utils::UUID> {
    using argument_type = utils::UUID;
    using result_type   = std::size_t;

    KobeBryant_NDAPI result_type operator()(const argument_type& obj) const;
};
} // namespace std