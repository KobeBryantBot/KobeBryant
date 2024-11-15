#pragma once
#include "Macros.hpp"
#include <iostream>
#include <optional>
#include <vector>

class Version {
protected:
    uint32_t mMajor;
    uint32_t mMinor;
    uint32_t mPatch;

public:
    KobeBryant_NDAPI static bool isValidVersionString(std::string const& version);

    KobeBryant_NDAPI static std::optional<Version> fromString(std::string const& version);

    KobeBryant_NDAPI static std::optional<Version> fromVector(std::vector<uint32_t> const& version);

    KobeBryant_NDAPI static Version getKobeBryantVersion();

    KobeBryant_NDAPI Version(uint32_t major, uint32_t minor, uint32_t patch);

    KobeBryant_NDAPI bool isInRange(Version const& minVersion, Version const& maxVersion) const;

    KobeBryant_NDAPI std::string toString(bool prefix = true) const;

    KobeBryant_NDAPI std::vector<uint32_t> toVector() const;

    KobeBryant_NDAPI bool operator==(const Version&) const;

    KobeBryant_NDAPI bool operator<(const Version&) const;

    KobeBryant_NDAPI bool operator<=(const Version&) const;

    KobeBryant_NDAPI bool operator>(const Version&) const;

    KobeBryant_NDAPI bool operator>=(const Version&) const;
};