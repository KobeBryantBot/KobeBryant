#include "api/i18n/base/LangFile.hpp"
#include "api/utils/FileUtils.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"
#include <fstream>

namespace i18n {

LangFile::LangFile(std::unordered_map<std::string, std::string> const& data) : mData(data) {}

LangFile LangFile::parse_file(const std::filesystem::path& filePath) {
    std::vector<std::string> lines;
    std::ifstream            file(filePath);
    auto                     result = LangFile();
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                utils::ReplaceStr(line, "\t", "");
                utils::ReplaceStr(line, "\\n", "\n");
                size_t equalPos = line.find("=");
                if (equalPos != std::string::npos) {
                    auto key = line.substr(0, equalPos);
                    utils::ReplaceStr(key, " ", "");
                    auto   right_key  = line.substr(equalPos + 1, line.size());
                    size_t commentPos = right_key.find("#");
                    auto   value      = right_key.substr(0, commentPos);
                    while (value.ends_with(" ")) {
                        value.erase(value.size() - 1);
                    }
                    if (!key.empty() && !value.empty()) {
                        result.mData[key] = value;
                    }
                }
            }
        }
        file.close();
    }
    return std::move(result);
}

std::vector<std::string> splitStringByNewline(const std::string& input) {
    std::vector<std::string> lines;
    size_t                   startPos = 0;
    while (startPos < input.size()) {
        size_t endPos = input.find_first_of("\n\r", startPos);
        if (endPos == std::string::npos) {
            lines.push_back(input.substr(startPos));
            break;
        }
        lines.push_back(input.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
    }
    return std::move(lines);
}

LangFile LangFile::parse(const std::string& data) {
    auto lines  = splitStringByNewline(data);
    auto result = LangFile();
    for (auto& line : lines) {
        if (!line.empty()) {
            utils::ReplaceStr(line, "\t", "");
            utils::ReplaceStr(line, "\\n", "\n");
            size_t equalPos = line.find("=");
            auto   key      = line.substr(0, equalPos);
            utils::ReplaceStr(key, " ", "");
            auto   right_key  = line.substr(equalPos + 1, line.size());
            size_t commentPos = right_key.find("#");
            auto   value      = right_key.substr(0, commentPos);
            while (value.ends_with(" ")) {
                value.erase(value.size() - 1);
            }
            if (!key.empty() && !value.empty()) {
                result.mData[key] = value;
            }
        }
    }
    return std::move(result);
}

LangFile LangFile::from_json(const std::string& jsonString) {
    auto json = nlohmann::json::parse(jsonString, nullptr, true, true);
    return from_json(json);
}

LangFile LangFile::from_json(const nlohmann::json& json) {
    std::unordered_map<std::string, std::string> languageMap;
    for (nlohmann::json::const_iterator it = json.begin(); it != json.end(); ++it) {
        if (it.value().is_string()) {
            languageMap[it.key()] = it.value().get<std::string>();
        }
    }
    return LangFile(languageMap);
}

std::string LangFile::dump() {
    std::map<std::string, std::string> map;
    for (auto& [key, lang] : mData) {
        map[key] = lang;
        utils::ReplaceStr(map[key], "\n", "\\n");
    }
    std::string result;
    for (auto& key : map) {
        result = result + key.first + "=" + key.second + "\n";
    }
    return std::move(result);
}

std::string LangFile::to_json_string(int format) { return to_json().dump(format); }

nlohmann::json LangFile::to_json() {
    nlohmann::json json;
    for (auto& [key, val] : mData) {
        json[key] = val;
    }
    return std::move(json);
}

bool LangFile::write_to_file(const std::filesystem::path& filePath) {
    std::ofstream newFile(filePath);
    if (newFile.is_open()) {
        newFile << dump();
        newFile.close();
        return true;
    }
    return false;
}

bool LangFile::has_value(const std::string& key) { return (bool)mData.count(key); }

void LangFile::set(const std::string& key, const std::string& value) { mData[key] = value; }

void LangFile::merge_patch(LangFile const& newData) {
    for (auto& key : newData.mData) {
        mData[key.first] = key.second;
    }
}

std::optional<std::string> LangFile::try_get(const std::string& key) {
    if (mData.contains(key)) {
        return mData[key];
    }
    return {};
}

bool LangFile::erase(const std::string& key) {
    if (has_value(key)) {
        mData.erase(key);
        return true;
    }
    return false;
}

void LangFile::erase(const std::vector<std::string>& keys) {
    for (auto& key : keys) {
        mData.erase(key);
    }
}

std::string
LangFile::get(const std::string& key, const std::vector<std::string>& param, const std::string& translateKeys) {
    auto value = try_get(key);
    if (value.has_value()) {
        auto result = value.value();
        if (param.empty()) {
            return result;
        }
        auto params = param;
        for (auto& info : params) {
            if (info.starts_with("%")) {
                info.erase(0, 1);
            }
        }
        if (translateKeys == "%0$s" && params.size() == 1) {
            utils::ReplaceStr(result, "%s", translate(params[0]));
        }
        for (int i = 0; i <= params.size() - 1; i++) {
            auto oldValue = translateKeys;
            utils::ReplaceStr(oldValue, "0", std::to_string(i + 1));
            utils::ReplaceStr(result, oldValue, translate(params[i]));
        }
        return result;
    }
    return key;
}

std::string
LangFile::translate(const std::string& key, const std::vector<std::string>& data, const std::string& translateKeys) {
    return get(key, data, translateKeys);
}

std::unordered_map<std::string, std::string>& LangFile::getTranslationMap() { return mData; }

} // namespace i18n