#include "api/i18n/LangI18n.hpp"
#include "api/utils/FileUtils.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"

namespace i18n {

LangI18n::LangI18n(const std::filesystem::path& languageDirectory, const std::string& languageCode)
: mLanguageDirectory(languageDirectory),
  mLanguageCode(languageCode) {}

LangI18n::~LangI18n() {
    for (auto& [langCode, langData] : mAllLanguages) {
        langData.reset();
    }
    mAllLanguages.clear();
}

bool LangI18n::loadOrCreateLanguage(const std::string& languageCode, std::shared_ptr<LangLanguage> language) {
    auto result                 = language->init();
    mAllLanguages[languageCode] = language;
    return result;
}

bool LangI18n::updateOrCreateLanguage(const std::string& languageCode, const std::string& language) {
    auto path = mLanguageDirectory / (languageCode + ".lang");
    auto lang = std::make_shared<LangLanguage>(path, language);
    return loadOrCreateLanguage(languageCode, lang);
}

bool LangI18n::updateOrCreateLanguage(const std::string& languageCode, const LangFile& language) {
    auto path = mLanguageDirectory / (languageCode + ".lang");
    auto lang = std::make_shared<LangLanguage>(path, language);
    return loadOrCreateLanguage(languageCode, lang);
}

bool LangI18n::loadAllLanguages() {
    bool result     = true;
    auto parentPath = mLanguageDirectory;
    auto files      = utils::getAllFileFullNameInDirectory(mLanguageDirectory);
    for (auto& file : files) {
        if (file.ends_with(".lang")) {
            auto code = file;
            utils::ReplaceStr(code, ".lang", "");
            if (!mAllLanguages.contains(code)) {
                auto        path = parentPath / file;
                std::string emptyLang;
                auto        language = std::make_shared<LangLanguage>(path, emptyLang);
                auto        temp     = language->init();
                result               = result && temp;
                mAllLanguages[code]  = language;
            }
        }
    }
    chooseLanguage(mLanguageCode);
    return result;
}

void LangI18n::reloadAllLanguages() {
    for (auto& [code, lang] : mAllLanguages) {
        lang->reload();
    }
}

bool LangI18n::chooseLanguage(const std::string& languageCode) {
    mLanguageCode = languageCode;
    if (mAllLanguages.contains(mLanguageCode)) {
        mLocalization = mAllLanguages[mLanguageCode];
        return true;
    }
    return false;
}

void LangI18n::setDefaultLanguage(const std::string& languageCode) { mDefaultLanguage = languageCode; }

std::string
LangI18n::translate(const std::string& key, const std::vector<std::string>& data, const std::string& translateKey) {
    if (!mLocalization) {
        chooseLanguage(mDefaultLanguage);
    }
    if (mLocalization) {
        if (mLocalization->has_value(key)) {
            return mLocalization->translate(key, data, translateKey);
        }
        if (mAllLanguages.contains(mDefaultLanguage)) {
            if (auto temp = mAllLanguages[mDefaultLanguage]) {
                return temp->translate(key, data, translateKey);
            }
        }
    }
    return key;
}

std::string LangI18n::translate(
    const std::string&              key,
    const std::string&              localLanguage,
    const std::vector<std::string>& data,
    const std::string&              translateKey
) {
    if (mAllLanguages.contains(localLanguage)) {
        if (auto temp = mAllLanguages[localLanguage]) {
            return temp->translate(key, data, translateKey);
        }
    } else if (mAllLanguages.contains(mDefaultLanguage)) {
        if (auto temp = mAllLanguages[mDefaultLanguage]) {
            return temp->translate(key, data, translateKey);
        }
    }
    return key;
}

std::string
LangI18n::get(const std::string& key, const std::vector<std::string>& data, const std::string& translateKey) {
    return translate(key, data, translateKey);
}

std::string LangI18n::get(
    const std::string&              key,
    const std::string&              localLanguage,
    const std::vector<std::string>& data,
    const std::string&              translateKey
) {
    return translate(key, localLanguage, data, translateKey);
}

void LangI18n::appendLanguage(const std::string& languageCode, const std::string& language) {
    auto data = LangFile::parse(language);
    return appendLanguage(languageCode, data);
}

void LangI18n::appendLanguage(const std::string& languageCode, const LangFile& language) {
    if (mAllLanguages.contains(languageCode)) {
        mAllLanguages[languageCode]->merge_patch(language);
    } else {
        mAllLanguages[languageCode] =
            std::make_shared<LangLanguage>(mLanguageDirectory / (languageCode + ".lang"), language);
    }
}

void LangI18n::forEachLangFile(
    const std::function<void(const std::string& languageCode, const LangLanguage& language)>& func
) {
    if (func) {
        for (auto& [code, data] : mAllLanguages) {
            func(code, *data);
        }
    }
}


} // namespace i18n