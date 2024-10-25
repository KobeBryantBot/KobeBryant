#include "api/utils/FileUtils.hpp"
#include <filesystem>

namespace utils {
namespace fs = std::filesystem;

std::optional<std::string> readFile(fs::path const& filePath, bool isBinary) {
    if (!fs::exists(filePath)) {
        return std::nullopt;
    }
    std::ifstream           fRead;
    std::ios_base::openmode mode = std::ios_base::in;
    if (isBinary) mode |= std::ios_base::binary;
    fRead.open(filePath, mode);
    if (!fRead.is_open()) {
        return std::nullopt;
    }
    std::string data((std::istreambuf_iterator<char>(fRead)), {});
    fRead.close();
    return data;
}

bool writeFile(fs::path const& filePath, std::string_view content, bool isBinary) {
    std::ofstream           fWrite;
    std::ios_base::openmode mode = std::ios_base::out;
    if (isBinary) mode |= std::ios_base::binary;
    std::error_code ec;
    fs::create_directories(filePath.parent_path(), ec);
    fWrite.open(filePath, mode);
    if (!fWrite.is_open()) {
        return false;
    }
    fWrite << content;
    fWrite.close();
    return true;
}

std::vector<std::string> getAllFileFullNameInDirectory(std::filesystem::path const& path) {
    std::vector<std::string> fileNames;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            fileNames.push_back(entry.path().filename().string());
        }
    }
    return fileNames;
}

std::vector<std::filesystem::path> getAllFileDirectories(std::filesystem::path const& path) {
    std::vector<std::filesystem::path> result;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            result.push_back(entry.path());
        }
    }
    return result;
}

} // namespace utils
