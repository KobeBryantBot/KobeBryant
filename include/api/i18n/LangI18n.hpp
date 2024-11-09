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
    LangI18n(std::filesystem::path const& languageDirectory, std::string const& languageCode = "en_US");

    LangI18n() = delete;

public:
    KobeBryant_API virtual ~LangI18n();

public:
    KobeBryant_API bool updateOrCreateLanguage(std::string const& languageCode, std::string const& language);

    KobeBryant_API bool updateOrCreateLanguage(std::string const& languageCode, LangFile const& language);

    KobeBryant_API void appendLanguage(std::string const& languageCode, std::string const& language);

    KobeBryant_API void appendLanguage(std::string const& languageCode, LangFile const& language);

    KobeBryant_API void
    forEachLangFile(std::function<void(std::string const& languageCode, LangLanguage const& language)> const& func);

    KobeBryant_API bool loadAllLanguages();

    KobeBryant_API void reloadAllLanguages();

    KobeBryant_API bool chooseLanguage(std::string const& languageCode = "en_US");

    KobeBryant_API void setDefaultLanguage(std::string const& languageCode = "en_US");

    KobeBryant_NDAPI std::string
    translate(std::string const& key, std::vector<std::string> const& params = {}, std::string const& data = "%0$s");

    KobeBryant_NDAPI std::string translate(
        std::string const&              key,
        std::string const&              localLanguage,
        std::vector<std::string> const& params = {},
        std::string const&              data   = "%0$s"
    );

    KobeBryant_NDAPI std::string
    get(std::string const& key, std::vector<std::string> const& params = {}, std::string const& data = "%0$s");

    KobeBryant_NDAPI std::string
                     get(std::string const&              key,
                         std::string const&              localLanguage,
                         std::vector<std::string> const& params = {},
                         std::string const&              data   = "%0$s");

private:
    bool loadOrCreateLanguage(std::string const& languageCode, std::shared_ptr<LangLanguage> language);
};


} // namespace i18n