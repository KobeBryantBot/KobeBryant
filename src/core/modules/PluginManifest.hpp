#pragma once
#include "Macros.hpp"
#include "api/Version.hpp"
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>

struct PluginDependence {
    std::string            mName;
    std::optional<Version> mMinVersion{};
    std::optional<Version> mMaxVersion{};

    static std::vector<PluginDependence> fromJson(const nlohmann::json& json);
};

struct PluginManifest {
    std::string                   mName;
    std::string                   mEntry;
    std::string                   mType{"native"};
    std::string                   mDescription{};
    Version                       mVersion{0, 0, 0};
    bool                          mPassive{false};
    std::vector<std::string>      mPreload{};
    std::vector<PluginDependence> mDependence{};
    std::vector<PluginDependence> mOptionalDependence{};

    static std::optional<PluginManifest> readFrom(const std::filesystem::path& path);
};