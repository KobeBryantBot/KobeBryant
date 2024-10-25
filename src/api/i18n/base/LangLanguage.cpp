#include "api/i18n/base/LangLanguage.hpp"
#include "core/Global.hpp"
#include "api/utils/FileUtils.hpp"

namespace i18n {

LangLanguage::LangLanguage(std::filesystem::path const& filePath, std::string const& defaultLanguage)
: mFilePath(filePath) {
    auto data = LangFile::parse(defaultLanguage);
    merge_patch(data);
}

LangLanguage::LangLanguage(std::filesystem::path const& filePath, LangFile const& defaultLanguage)
: mFilePath(filePath) {
    merge_patch(defaultLanguage);
}

LangLanguage::LangLanguage(
    std::filesystem::path const&                        filePath,
    std::unordered_map<std::string, std::string> const& defaultLanguage
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