#include "api/utils/FunctionUtils.hpp"
#include "core/Global.hpp"
#include <iostream>
#include <psapi.h>
#include <tchar.h>
#include <tlhelp32.h>

namespace utils {

FARPROC GetFunctionAddress(LPCWSTR dllName, LPCSTR funcName) {
    if (HMODULE hModule = GetModuleHandle(dllName); hModule) {
        return GetProcAddress(hModule, funcName);
    }
    return nullptr;
}

FARPROC GetFunctionAddress(LPCSTR funcName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return nullptr;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(hProcessSnap, &me32)) {
        CloseHandle(hProcessSnap);
        return nullptr;
    }

    while (Module32Next(hProcessSnap, &me32)) {
        HMODULE hMod = GetModuleHandle(me32.szModule);
        if (hMod) {
            auto result = GetProcAddress(hMod, funcName);
            if (result) {
                CloseHandle(hProcessSnap);
                return result;
            }
        }
    }

    CloseHandle(hProcessSnap);
    return nullptr;
}

} // namespace utils