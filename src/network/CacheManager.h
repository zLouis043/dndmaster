#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

class CacheManager {
private:
    std::filesystem::path m_cacheDir;

    static std::string calculateHash(const std::vector<uint8_t>& data);

public:
    CacheManager(const std::string& cacheFolder = "Cache");

    std::string saveAsset(const std::vector<uint8_t>& data, const std::string& extension = ".webp");

    bool hasAsset(const std::string& hash) const;

    bool loadAsset(const std::string& hash, std::vector<uint8_t>& outData);

    void cleanupOldFiles(int maxDays = 30);
};