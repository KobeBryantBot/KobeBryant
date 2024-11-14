#pragma once
#include "Global.hpp"
#include "fmt/color.h"
#include "fmt/format.h"

#define KOBE_VERSION_MAJOR 0
#define KOBE_VERSION_MINOR 6
#define KOBE_VERSION_PATCH 6

#define KOBE_VERSION_STRING                                                                                            \
    fmt::format("{}-{}.{}.{}", BOT_NAME, KOBE_VERSION_MAJOR, KOBE_VERSION_MINOR, KOBE_VERSION_PATCH)

#define KOBE_VERSION_COLOR_STRING fmt::format(fg(fmt::color::pink), fmt::runtime(KOBE_VERSION_STRING))