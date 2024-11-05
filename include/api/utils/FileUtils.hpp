#pragma once
#include "Macros.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace utils {

KobeBryant_NDAPI std::optional<std::string> readFile(std::filesystem::path const& filePath, bool isBinary = false);

KobeBryant_NDAPI std::optional<std::vector<uint8_t>> readBinaryFile(std::filesystem::path const& filePath);

KobeBryant_API bool writeFile(std::filesystem::path const& filePath, std::string_view content, bool isBinary = false);

KobeBryant_API bool writeBinaryFile(std::filesystem::path const& filePath, std::vector<uint8_t> const& content);

KobeBryant_NDAPI std::vector<std::string> getAllFileFullNameInDirectory(std::filesystem::path const& path);

KobeBryant_NDAPI std::vector<std::filesystem::path> getAllFileDirectories(std::filesystem::path const& path);

} // namespace utils
