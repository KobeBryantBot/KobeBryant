#pragma once
#include "Macros.hpp"
#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

struct PluginManifest {
    std::string              mName;
    std::string              mEntry;
    std::string              mType               = "native";
    bool                     mPassive            = false;
    std::vector<std::string> mDependence         = {};
    std::vector<std::string> mOptionalDependence = {};

    static std::optional<PluginManifest> readFrom(std::filesystem::path const& path);
};