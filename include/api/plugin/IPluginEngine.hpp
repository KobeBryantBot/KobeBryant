#pragma once
#include "Macros.hpp"
#include <filesystem>
#include <iostream>

class IPluginEngine {
public:
    IPluginEngine() = default;

    virtual ~IPluginEngine() = default;

    virtual std::string getPluginType() const = 0;

    virtual bool loadPlugin(std::string const& plugin, std::filesystem::path const& entry) = 0;

    virtual bool unloadPlugin(std::string const& plugin) = 0;
};
