#pragma once
#include "Macros.hpp"
#include "api/i18n/base/LangFile.hpp"

namespace i18n {

class LangLanguage : public LangFile {
    friend LangFile;

private:
    std::filesystem::path mFilePath;

public:
    KobeBryant_NDAPI LangLanguage(const std::filesystem::path& filePath, const std::string& defaultLanguage);

    KobeBryant_NDAPI LangLanguage(const std::filesystem::path& filePath, const LangFile& defaultLanguage);

    KobeBryant_NDAPI LangLanguage(
        const std::filesystem::path&                        filePath,
        const std::unordered_map<std::string, std::string>& defaultLanguage
    );

public:
    KobeBryant_API bool init();

    KobeBryant_API bool reload();

    KobeBryant_API bool save_file();
};

} // namespace i18n