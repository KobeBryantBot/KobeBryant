#include "PluginManifest.hpp"
#include "api/utils/FileUtils.hpp"
#include "core/modules/KobeBryant.hpp"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;

std::optional<PluginManifest> PluginManifest::readFrom(std::filesystem::path const& path) {
    try {
        if (auto file = utils::readFile(path)) {
            auto data = nlohmann::json::parse(*file, nullptr, true, true);
            if (data.contains("name") && data.contains("entry")) {
                PluginManifest result;
                result.mName  = data["name"];
                result.mEntry = data["entry"];
                if (result.mName.empty() || result.mEntry.empty()) {
                    return {};
                }
                if (data.contains("type")) {
                    result.mType = data["type"];
                }
                if (data.contains("passive")) {
                    result.mPassive = data["passive"];
                }
                if (data.contains("dependence")) {
                    result.mDependence = data["dependence"];
                }
                if (data.contains("optional_dependence")) {
                    result.mOptionalDependence = data["optional_dependence"];
                }
                return result;
            }
        }
    }
    CATCH
    return {};
}