#pragma once
#include "api/Logger.hpp"
#include "api/utils/FileUtils.hpp"
#include "nlohmann/json.hpp"
#include <Windows.h>
#include <atomic>
#include <iostream>
#include <thread>

#define BOT_NAME "KobeBryant"

#define S(x)     std::to_string(x)
#define PRINT(x) std::cout << x << std::endl

extern std::atomic<bool> EXIST_FLAG;

#define KOBE_VERSION_MAJOR 0
#define KOBE_VERSION_MINOR 3
#define KOBE_VERSION_PATCH 0

extern std::string tr(std::string const& key, std::vector<std::string> const& params = {});