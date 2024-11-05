#pragma once
#include "Macros.hpp"
#include <Windows.h>
#include <iostream>
#include <optional>
#include <vector>

namespace utils {

[[nodiscard]] inline HMODULE getCurrentModuleHandle() {
    HMODULE hModule = NULL;
    if (GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)getCurrentModuleHandle,
            &hModule
        )
        == FALSE) {
        return NULL;
    }
    return hModule;
}

KobeBryant_NDAPI std::optional<std::string> readResource(HMODULE hModule, int id, bool isBinary = false);

KobeBryant_NDAPI std::optional<std::vector<uint8_t>> readBinaryResource(HMODULE hModule, int id);

[[nodiscard]] inline std::optional<std::string> readCurrentResource(int id, bool isBinary = false) {
    auto hModule = getCurrentModuleHandle();
    return readResource(hModule, id, isBinary);
}

[[nodiscard]] inline std::optional<std::vector<uint8_t>> readCurrentBinaryResource(int id) {
    auto hModule = getCurrentModuleHandle();
    return readBinaryResource(hModule, id);
}

} // namespace utils
