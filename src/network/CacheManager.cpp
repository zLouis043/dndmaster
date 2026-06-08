#include "CacheManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>

namespace fs = std::filesystem;

CacheManager::CacheManager(const std::string& cacheFolder) 
    : m_cacheDir(cacheFolder) 
{
    if (!fs::exists(m_cacheDir)) {
        fs::create_directories(m_cacheDir);
    }
}

std::string CacheManager::calculateHash(const std::vector<uint8_t>& data) {
    uint64_t hash = 14695981039346656037ull;
    for (uint8_t byte : data) {
        hash ^= byte;
        hash *= 1099511628211ull;
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash 
       << "-" << data.size();
    return ss.str();
}

std::string CacheManager::saveAsset(const std::vector<uint8_t>& data, const std::string& extension) {
    if (data.empty()) return "";

    std::string hash = calculateHash(data);
    std::string filename = hash + extension;
    fs::path filePath = m_cacheDir / filename;

    if (!fs::exists(filePath)) {
        std::ofstream file(filePath, std::ios::binary);
        if (file) {
            file.write(reinterpret_cast<const char*>(data.data()), data.size());
        }
    } else {
        fs::last_write_time(filePath, fs::file_time_type::clock::now());
    }

    return hash;
}

bool CacheManager::hasAsset(const std::string& hash) const {
    for (const auto& entry : fs::directory_iterator(m_cacheDir)) {
        if (entry.path().stem().string() == hash) {
            return true;
        }
    }
    return false;
}

bool CacheManager::loadAsset(const std::string& hash, std::vector<uint8_t>& outData) {
    fs::path targetPath;
    
    for (const auto& entry : fs::directory_iterator(m_cacheDir)) {
        if (entry.path().stem().string() == hash) {
            targetPath = entry.path();
            break;
        }
    }

    if (targetPath.empty()) return false;

    std::ifstream file(targetPath, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    outData.resize(size);
    if (file.read(reinterpret_cast<char*>(outData.data()), size)) {
        file.close();
        fs::last_write_time(targetPath, fs::file_time_type::clock::now());
        return true;
    }
    return false;
}

void CacheManager::cleanupOldFiles(int maxDays) {
    auto now = fs::file_time_type::clock::now();
    
    for (const auto& entry : fs::directory_iterator(m_cacheDir)) {
        if (entry.is_regular_file()) {
            auto ftime = fs::last_write_time(entry);
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - ftime).count();
            
            if (age > (maxDays * 24)) {
                std::cout << "[CACHE] Eliminato file obsoleto (TTL scaduto): " << entry.path().filename() << "\n";
                fs::remove(entry);
            }
        }
    }
}