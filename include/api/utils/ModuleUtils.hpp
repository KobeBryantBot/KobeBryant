#pragma once
#include "Macros.hpp"
#include <Windows.h>
#include <iostream>
#include <optional>

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

[[nodiscard]] inline std::optional<std::string> readCurrentResource(int id, bool isBinary = false) {
    auto hModule = getCurrentModuleHandle();
    return readResource(hModule, id, isBinary);
}

} // namespace utils
