#pragma once
#include "api/i18n/base/LangLanguage.hpp"
#include <functional>

namespace i18n {

class LangI18n {
    friend LangLanguage;

private:
    std::filesystem::path                                          mLanguageDirectory;
    std::unordered_map<std::string, std::shared_ptr<LangLanguage>> mAllLanguages;
    std::string                                                    mLanguageCode;
    std::shared_ptr<LangLanguage>                                  mLocalization    = nullptr;
    std::string                                                    mDefaultLanguage = "en_US";

public:
    KobeBryant_NDAPI
    LangI18n(const std::filesystem::path& languageDirectory, const std::string& languageCode = "en_US");

    LangI18n() = delete;

public:
    KobeBryant_API virtual ~LangI18n();

public:
    KobeBryant_API bool updateOrCreateLanguage(const std::string& languageCode, const std::string& language);

    KobeBryant_API bool updateOrCreateLanguage(const std::string& languageCode, const LangFile& language);

    KobeBryant_API void appendLanguage(const std::string& languageCode, const std::string& language);

    KobeBryant_API void appendLanguage(const std::string& languageCode, const LangFile& language);

    KobeBryant_API void
    forEachLangFile(const std::function<void(const std::string& languageCode, const LangLanguage& language)>& func);

    KobeBryant_API bool loadAllLanguages();

    KobeBryant_API void reloadAllLanguages();

    KobeBryant_API bool chooseLanguage(const std::string& languageCode = "en_US");

    KobeBryant_API void setDefaultLanguage(const std::string& languageCode = "en_US");

    KobeBryant_NDAPI std::string
    translate(const std::string& key, const std::vector<std::string>& params = {}, const std::string& data = "%0$s");

    KobeBryant_NDAPI std::string translate(
        const std::string&              key,
        const std::string&              localLanguage,
        const std::vector<std::string>& params = {},
        const std::string&              data   = "%0$s"
    );

    KobeBryant_NDAPI std::string
    get(const std::string& key, const std::vector<std::string>& params = {}, const std::string& data = "%0$s");

    KobeBryant_NDAPI std::string
                     get(const std::string&              key,
                         const std::string&              localLanguage,
                         const std::vector<std::string>& params = {},
                         const std::string&              data   = "%0$s");

private:
    bool loadOrCreateLanguage(const std::string& languageCode, std::shared_ptr<LangLanguage> language);
};


} // namespace i18n