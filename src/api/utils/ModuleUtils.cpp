#include "api/utils/ModuleUtils.hpp"

namespace utils {

std::optional<std::string> readResource(HMODULE hModule, int id, bool isBinary) {
    if (HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(id), isBinary ? L"BINFILE" : L"TEXTFILE")) {
        if (HGLOBAL resource = LoadResource(hModule, hRes)) {
            void*       data = LockResource(resource);
            DWORD       size = SizeofResource(hModule, hRes);
            std::string outData(static_cast<const char*>(data), size);
            return std::move(outData);
        }
    }
    return {};
}

} // namespace utils
