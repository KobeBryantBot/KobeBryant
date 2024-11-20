#pragma once
#include "Macros.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

namespace i18n {

class LangFile {
private:
    std::unordered_map<std::string, std::string> mData;

public:
    KobeBryant_NDAPI LangFile(const std::unordered_map<std::string, std::string>& data);

    KobeBryant_NDAPI LangFile() = default;

public:
    virtual ~LangFile() = default;

public:
    KobeBryant_NDAPI static LangFile parse(const std::string& data);

    KobeBryant_NDAPI static LangFile parse_file(const std::filesystem::path& filePath);

    KobeBryant_NDAPI static LangFile from_json(const std::string& jsonString);

    KobeBryant_NDAPI static LangFile from_json(const nlohmann::json& json);

public:
    KobeBryant_NDAPI std::optional<std::string> try_get(const std::string& key);

    KobeBryant_NDAPI std::string
    translate(const std::string& key, const std::vector<std::string>& param = {}, const std::string& data = "%0$s");

    KobeBryant_NDAPI std::string
    get(const std::string& key, const std::vector<std::string>& param = {}, const std::string& data = "%0$s");

    KobeBryant_API void set(const std::string& key, const std::string& value);

    KobeBryant_API bool erase(const std::string& key);

    KobeBryant_API void erase(const std::vector<std::string>& keys);

    KobeBryant_NDAPI bool has_value(const std::string& key);

    KobeBryant_API void merge_patch(const LangFile& data);

    KobeBryant_NDAPI std::string dump();

    KobeBryant_NDAPI std::string to_json_string(int format = 4);

    KobeBryant_NDAPI nlohmann::json to_json();

    KobeBryant_API bool write_to_file(const std::filesystem::path& filePath);

    KobeBryant_NDAPI std::unordered_map<std::string, std::string>& getTranslationMap();
};

} // namespace i18n