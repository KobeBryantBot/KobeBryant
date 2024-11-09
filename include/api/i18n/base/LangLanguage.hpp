#pragma once
#include "Macros.hpp"
#include "api/i18n/base/LangFile.hpp"

namespace i18n {

class LangLanguage : public LangFile {
    friend LangFile;

private:
    std::filesystem::path mFilePath;

public:
    KobeBryant_NDAPI LangLanguage(std::filesystem::path const& filePath, std::string const& defaultLanguage);

    KobeBryant_NDAPI LangLanguage(std::filesystem::path const& filePath, LangFile const& defaultLanguage);

    KobeBryant_NDAPI LangLanguage(
        std::filesystem::path const&                        filePath,
        std::unordered_map<std::string, std::string> const& defaultLanguage
    );

public:
    KobeBryant_API bool init();

    KobeBryant_API bool reload();

    KobeBryant_API bool save_file();
};

} // namespace i18n