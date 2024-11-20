#pragma once
#include "Macros.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace utils {

KobeBryant_NDAPI std::optional<std::string> readFile(const std::filesystem::path& filePath, bool isBinary = false);

KobeBryant_NDAPI std::optional<std::vector<uint8_t>> readBinaryFile(const std::filesystem::path& filePath);

KobeBryant_API bool writeFile(const std::filesystem::path& filePath, std::string_view content, bool isBinary = false);

KobeBryant_API bool writeBinaryFile(const std::filesystem::path& filePath, const std::vector<uint8_t>& content);

KobeBryant_NDAPI std::vector<std::string> getAllFileFullNameInDirectory(const std::filesystem::path& path);

KobeBryant_NDAPI std::vector<std::filesystem::path> getAllFileDirectories(const std::filesystem::path& path);

} // namespace utils
