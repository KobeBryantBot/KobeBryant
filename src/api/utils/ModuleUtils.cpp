#include "api/utils/ModuleUtils.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/modules/PluginManager.hpp"

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

std::optional<std::vector<uint8_t>> readBinaryResource(HMODULE hModule, int id) {
    if (auto data = readResource(hModule, id, true)) {
        return utils::toBinaryArray(data.value());
    }
    return std::nullopt;
}

std::string getPluginModuleName(HMODULE hModule) {
    char lpFilename[MAX_PATH];
    GetModuleFileNameA(hModule, lpFilename, sizeof(lpFilename));
    std::string        pluginName = lpFilename;
    static std::string parentPath = std::filesystem::absolute("./plugins/").string();
    utils::ReplaceStr(pluginName, parentPath, "");
    auto pos = pluginName.find("\\");
    if (pos != std::string::npos) {
        pluginName.erase(pos);
    }
    return pluginName;
}

} // namespace utils
