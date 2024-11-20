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

extern std::string tr(const std::string& key, const std::vector<std::string>& params = {});

extern void backupLog();