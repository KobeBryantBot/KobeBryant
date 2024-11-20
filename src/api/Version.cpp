#include "api/Version.hpp"
#include "core/Version.hpp"
#include <fmt/format.h>
#include <regex>

bool Version::isValidVersionString(const std::string& version) {
    std::regex pattern("(v)?(\\d+)\\.(\\d+)\\.(\\d+)");
    return std::regex_match(version, pattern);
}

std::optional<Version> Version::fromString(const std::string& version) {
    if (isValidVersionString(version)) {
        std::istringstream iss(version);
        char               prefix;
        int                major, minor, patch;
        if (iss.peek() == 'v') {
            iss >> prefix >> major;
        } else {
            iss >> major;
        }
        iss.ignore(1);
        iss >> minor;
        iss.ignore(1);
        iss >> patch;
        return Version(major, minor, patch);
    }
    return {};
}

std::optional<Version> Version::fromVector(const std::vector<uint32_t>& version) {
    if (version.size() == 3) {
        return Version(version[0], version[1], version[2]);
    }
    return {};
}

Version Version::getKobeBryantVersion() { return Version(KOBE_VERSION_MAJOR, KOBE_VERSION_MINOR, KOBE_VERSION_PATCH); }

Version::Version(uint32_t major, uint32_t minor, uint32_t patch) : mMajor(major), mMinor(minor), mPatch(patch) {}

bool Version::isInRange(const Version& minVersion, const Version& maxVersion) const {
    return (*this >= minVersion) && (*this <= maxVersion);
}

std::string Version::toString(bool prefix) const {
    auto result = fmt::format("{}.{}.{}", mMajor, mMinor, mPatch);
    if (prefix) {
        return "v" + result;
    }
    return result;
}

std::vector<uint32_t> Version::toVector() const { return {mMajor, mMinor, mPatch}; }

bool Version::operator==(const Version& other) const {
    return (mMajor == other.mMajor) && (mMinor == other.mMinor) && (mPatch == other.mPatch);
}

bool Version::operator<(const Version& other) const {
    if (mMajor != other.mMajor) {
        return mMajor < other.mMajor;
    }
    if (mMinor != other.mMinor) {
        return mMinor < other.mMinor;
    }
    return mPatch < other.mPatch;
}

bool Version::operator<=(const Version& other) const { return !(*this > other); }

bool Version::operator>(const Version& other) const { return other < *this; }

bool Version::operator>=(const Version& other) const { return !(*this < other); }