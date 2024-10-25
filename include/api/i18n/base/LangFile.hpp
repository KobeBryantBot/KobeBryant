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
    KobeBryant_NDAPI LangFile(std::unordered_map<std::string, std::string> const& data);

    KobeBryant_NDAPI LangFile() = default;

public:
    virtual ~LangFile() = default;

public:
    KobeBryant_NDAPI static LangFile parse(std::string const& data);

    KobeBryant_NDAPI static LangFile parse_file(std::filesystem::path const& filePath);

    KobeBryant_NDAPI static LangFile from_json(std::string const& jsonString);

    KobeBryant_NDAPI static LangFile from_json(nlohmann::json const& json);

public:
    KobeBryant_NDAPI std::optional<std::string> try_get(std::string const& key);

    KobeBryant_NDAPI std::string
    translate(std::string const& key, std::vector<std::string> const& param = {}, std::string const& data = "%0$s");

    KobeBryant_NDAPI std::string
    get(std::string const& key, std::vector<std::string> const& param = {}, std::string const& data = "%0$s");

    KobeBryant_API void set(std::string const& key, std::string const& value);

    KobeBryant_API bool erase(std::string const& key);

    KobeBryant_API void erase(std::vector<std::string> const& keys);

    KobeBryant_NDAPI bool has_value(std::string const& key);

    KobeBryant_API void merge_patch(LangFile const& data);

    KobeBryant_NDAPI std::string dump();

    KobeBryant_NDAPI std::string to_json_string(int format = 4);

    KobeBryant_NDAPI nlohmann::json to_json();

    KobeBryant_API bool write_to_file(std::filesystem::path const& filePath);

    KobeBryant_NDAPI std::unordered_map<std::string, std::string>& getTranslationMap();
};

} // namespace i18n