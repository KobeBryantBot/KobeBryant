#include "api/i18n/base/LangLanguage.hpp"
#include "api/utils/FileUtils.hpp"
#include "core/Global.hpp"

namespace i18n {

LangLanguage::LangLanguage(const std::filesystem::path& filePath, const std::string& defaultLanguage)
: mFilePath(filePath) {
    auto data = LangFile::parse(defaultLanguage);
    merge_patch(data);
}

LangLanguage::LangLanguage(const std::filesystem::path& filePath, const LangFile& defaultLanguage)
: mFilePath(filePath) {
    merge_patch(defaultLanguage);
}

LangLanguage::LangLanguage(
    const std::filesystem::path&                        filePath,
    const std::unordered_map<std::string, std::string>& defaultLanguage
)
: mFilePath(filePath) {
    merge_patch(LangFile(defaultLanguage));
}

bool LangLanguage::init() {
    auto dirPath = std::filesystem::path(mFilePath).parent_path();
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
    }
    if (std::filesystem::exists(mFilePath)) {
        auto oldData = LangFile::parse_file(mFilePath);
        merge_patch(oldData);
    }
    return write_to_file(mFilePath);
}

bool LangLanguage::reload() {
    auto file = utils::readFile(mFilePath);
    if (file.has_value()) {
        auto newData = LangFile::parse(file.value());
        merge_patch(newData);
        return save_file();
    }
    return false;
}

bool LangLanguage::save_file() { return write_to_file(mFilePath); }

} // namespace i18n