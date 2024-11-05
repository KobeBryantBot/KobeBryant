#pragma once
#include "Macros.hpp"
#include <iostream>

class IPluginEngine {
public:
    IPluginEngine() = default;

    virtual ~IPluginEngine() = default;

    virtual std::string getPluginType() const = 0;

    virtual bool loadPlugin(std::string const& plugin) = 0;

    virtual bool unloadPlugin(std::string const& plugin) = 0;
};
