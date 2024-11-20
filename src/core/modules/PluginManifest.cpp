#include "PluginManifest.hpp"
#include "api/utils/FileUtils.hpp"
#include "core/modules/KobeBryant.hpp"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;

std::vector<PluginDependence> PluginDependence::fromJson(const nlohmann::json& json) {
    try {
        if (json.is_array()) {
            std::vector<PluginDependence> result;
            for (auto& depe : json) {
                if (depe.is_object()) {
                    if (depe.contains("name")) {
                        std::string name = depe["name"];
                        if (name.empty()) {
                            continue;
                        }
                        auto info  = PluginDependence();
                        info.mName = name;
                        if (depe.contains("min_version") && depe["min_version"].is_string()) {
                            info.mMinVersion = Version::fromString(depe["min_version"]);
                        }
                        if (depe.contains("max_version") && depe["max_version"].is_string()) {
                            info.mMaxVersion = Version::fromString(depe["max_version"]);
                        }
                        result.push_back(info);
                    }
                }
            }
            return result;
        }
    }
    CATCH
    return {};
}

std::optional<PluginManifest> PluginManifest::readFrom(const std::filesystem::path& path) {
    try {
        if (auto file = utils::readFile(path)) {
            auto data = nlohmann::json::parse(*file, nullptr, true, true);
            if (data.contains("name") && data["name"].is_string() && data.contains("entry")
                && data["entry"].is_string()) {
                PluginManifest result;
                result.mName  = data["name"];
                result.mEntry = data["entry"];
                if (result.mName.empty() || result.mEntry.empty()) {
                    return {};
                }
                if (data.contains("type") && data["type"].is_string()) {
                    result.mType = data["type"];
                }
                if (data.contains("version") && data["version"].is_string()) {
                    std::string verStr = data["version"];
                    if (auto ver = Version::fromString(verStr)) {
                        result.mVersion = *ver;
                    }
                }
                if (data.contains("description") && data["description"].is_string()) {
                    result.mDescription = data["description"];
                }
                if (data.contains("passive") && data["passive"].is_boolean()) {
                    result.mPassive = data["passive"];
                }
                if (data.contains("preload") && data["preload"].is_array()) {
                    result.mPreload = data["preload"];
                }
                if (data.contains("dependence")) {
                    result.mDependence = PluginDependence::fromJson(data["dependence"]);
                }
                if (data.contains("optional_dependence")) {
                    result.mOptionalDependence = PluginDependence::fromJson(data["optional_dependence"]);
                }
                return result;
            }
        }
    }
    CATCH
    return {};
}