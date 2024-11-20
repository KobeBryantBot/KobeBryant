#pragma once
#include "Macros.hpp"
#include <filesystem>
#include <iostream>

class IPluginEngine {
public:
    IPluginEngine() = default;

    virtual ~IPluginEngine() = default;

    virtual std::string getPluginType() const = 0;

    virtual bool loadPlugin(const std::string& plugin, const std::filesystem::path& entry) = 0;

    virtual bool unloadPlugin(const std::string& plugin) = 0;
};
