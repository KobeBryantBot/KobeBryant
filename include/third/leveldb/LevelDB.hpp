#pragma once
#include <filesystem>
#include <functional>
#include <iostream>
#include <optional>

class LevelDB {
private:
    enum CompressionType {
        kNoCompression      = 0x0,
        kSnappyCompression  = 0x1,
        kZstdCompression    = 0x2,
        kZLibRawCompression = 0x4,
    };

public:
    [[nodiscard]] explicit LevelDB(std::filesystem::path const& path);

    [[nodiscard]] explicit LevelDB(
        std::filesystem::path const& path,
        bool                         createIfMiss,
        bool                         fixIfError,
        int                          bloomFilterBit,
        CompressionType              compressionType
    );

    LevelDB(LevelDB const&) noexcept = delete;

    LevelDB& operator=(LevelDB const&) noexcept = delete;

    [[nodiscard]] LevelDB(LevelDB&&) noexcept;

    LevelDB& operator=(LevelDB&&) noexcept;

    ~LevelDB();

    [[nodiscard]] std::optional<std::string> get(std::string_view key) const;

    [[nodiscard]] bool has(std::string_view key) const;

    [[nodiscard]] bool empty() const;

    bool set(std::string_view key, std::string_view val);

    bool del(std::string_view key);

    void forEach(std::function<void(std::string_view, std::string_view)> const& fn) const;
};
