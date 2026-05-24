#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>

template <typename T>
struct AssetTraits {
    static T load(const std::string& path) {
        static_assert(sizeof(T) == 0, "[ASSET ENGINE] Errore: AssetTraits::load non specializzato!");
        return T{};
    }
    
    static T loadFromMemory(const std::vector<uint8_t>& data) {
        static_assert(sizeof(T) == 0, "[ASSET ENGINE] Errore: AssetTraits::loadFromMemory non specializzato!");
        return T{};
    }
};

class IAssetCache {
public:
    virtual ~IAssetCache() = default;
    virtual void clear() = 0;
};

template <typename T>
class AssetCache : public IAssetCache {
    std::unordered_map<std::string, T> m_assets;
    T m_fallback{};
    
public:
    void add(const std::string& id, const T& asset) { m_assets[id] = asset; }

    T& get(const std::string& id) {
        auto it = m_assets.find(id);
        if (it != m_assets.end()) return it->second;
        return m_fallback; 
    }

    const T& getConst(const std::string& id) const {
        auto it = m_assets.find(id);
        if (it != m_assets.end()) return it->second;
        return m_fallback;
    }

    bool has(const std::string& id) const { return m_assets.count(id) > 0; }
    void remove(const std::string& id) { m_assets.erase(id); }
    void clear() override { m_assets.clear(); }
};

class AssetManager {
private:
    std::unordered_map<std::type_index, std::shared_ptr<IAssetCache>> m_caches;
    std::string m_basePath = "";

    template <typename T>
    AssetCache<T>* getCache() {
        auto it = m_caches.find(typeid(T));
        if (it == m_caches.end()) {
            auto cache = std::make_shared<AssetCache<T>>();
            m_caches[typeid(T)] = cache;
            return cache.get();
        }
        return static_cast<AssetCache<T>*>(it->second.get());
    }

    template <typename T>
    const AssetCache<T>* getCacheConst() const {
        auto it = m_caches.find(typeid(T));
        if (it != m_caches.end()) {
            return static_cast<const AssetCache<T>*>(it->second.get());
        }
        return nullptr;
    }

public:
    void setBasePath(const std::string& path) {
        m_basePath = path;
        if (!m_basePath.empty() && m_basePath.back() != '/' && m_basePath.back() != '\\') {
            m_basePath += '/';
        }
    }

    template <typename T>
    bool load(const std::string& id, const std::string& path) {
        std::string fullPath = m_basePath + path;

        T asset = AssetTraits<T>::load(fullPath);
        if constexpr (std::is_convertible_v<T, bool> || std::is_constructible_v<bool, T>) {
            if (!asset) return false;
        }
        getCache<T>()->add(id, asset);
        return true;
    }

    template <typename T>
    bool loadFromMemory(const std::string& id, const std::vector<uint8_t>& data) {
        T asset = AssetTraits<T>::loadFromMemory(data);
        if constexpr (std::is_convertible_v<T, bool> || std::is_constructible_v<bool, T>) {
            if (!asset) return false;
        }
        getCache<T>()->add(id, asset);
        return true;
    }

    template <typename T>
    void store(const std::string& id, const T& asset) { getCache<T>()->add(id, asset); }

    template <typename T>
    T& get(const std::string& id) { 
        return getCache<T>()->get(id); 
    }

    template <typename T>
    const T& get(const std::string& id) const { 
        const auto* cache = getCacheConst<T>();
        if (cache) return cache->getConst(id);
        
        static T fallback{};
        return fallback;
    }

    template <typename T>
    bool has(const std::string& id) const {
        const auto* cache = getCacheConst<T>();
        return cache ? cache->has(id) : false;
    }

    template <typename T>
    void unload(const std::string& id) { getCache<T>()->remove(id); }

    void clearAll() {
        for (auto& pair : m_caches) pair.second->clear();
    }
};