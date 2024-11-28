#pragma once
#include "Macros.hpp"
#include <iostream>
#include <optional>
#include <vector>

class Version {
protected:
    uint32_t mMajor;
    uint32_t mMinor;
    uint32_t mPatch;

public:
    /**
     * @brief 检查版本字符串是否有效
     * @param version 版本字符串
     * @return 是否有效
     */
    KobeBryant_NDAPI static bool isValidVersionString(const std::string& version);

    /**
     * @brief 从版本字符串创建版本对象
     * @param version 版本字符串
     * @return 版本对象
     */
    KobeBryant_NDAPI static std::optional<Version> fromString(const std::string& version);

    /**
     * @brief 从版本数组创建版本对象
     * @param version 版本数组
     * @return 版本对象
     */
    KobeBryant_NDAPI static std::optional<Version> fromVector(const std::vector<uint32_t>& version);

    /**
     * @brief 获取机器人当前版本
     * @return 机器人当前版本
     */
    KobeBryant_NDAPI static Version getKobeBryantVersion();

    /**
     * @brief 构造函数
     * @param major 主版本号
     * @param minor 次版本号
     * @param patch 修订版本号
     * @return 版本对象
     */
    KobeBryant_NDAPI Version(uint32_t major, uint32_t minor, uint32_t patch);

    /**
     * @brief 检查版本是否在指定范围内
     * @param minVersion 最小版本
     * @param maxVersion 最大版本
     * @return 是否在指定范围内
     */
    KobeBryant_NDAPI bool isInRange(const Version& minVersion, const Version& maxVersion) const;

    /**
     * @brief 将版本对象转换为字符串
     * @param prefix 是否添加前缀
     * @return 版本字符串
     */
    KobeBryant_NDAPI std::string toString(bool prefix = true) const;

    /**
     * @brief 将版本对象转换为版本数组
     * @return 版本数组
     */
    KobeBryant_NDAPI std::vector<uint32_t> toVector() const;

    KobeBryant_NDAPI bool operator==(const Version&) const;
    KobeBryant_NDAPI bool operator<(const Version&) const;
    KobeBryant_NDAPI bool operator<=(const Version&) const;
    KobeBryant_NDAPI bool operator>(const Version&) const;
    KobeBryant_NDAPI bool operator>=(const Version&) const;
};