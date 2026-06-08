#pragma once
#include "SessionHost.h"
#include "SessionClient.h"
#include "WsTransport.h"
#include "CacheManager.h"
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <memory>
#include <string>
#include <functional>

class NetworkManager {
public:
    using OnMessage     = std::function<void(const NetMessage&)>;
    using OnPlayerJoin  = std::function<bool(const std::string&, const std::string&)>;
    using OnPlayerLeave = std::function<void(const std::string&)>;
    using OnReady       = std::function<void(const std::string&)>;
    using OnError       = std::function<void(const std::string&)>;
    using OnAssetReady = std::function<void(const std::string& hash, const std::vector<uint8_t>& data)>;

private:
    std::unique_ptr<SessionHost>   m_host;
    std::unique_ptr<SessionClient> m_client;

    CacheManager m_cache;

    std::mutex m_assetMutex;
    std::queue<std::pair<std::string, std::vector<uint8_t>>> m_readyAssets;
    OnAssetReady m_onAssetReady;

    static constexpr const char* WS_URL = "wss://dndmaster-ws.fly.dev";
public:

    NetworkManager() : m_cache("Cache") {}

    void setOnAssetReady(OnAssetReady cb) { m_onAssetReady = std::move(cb); }

    bool hostSession(OnPlayerJoin  onJoin,
                     OnPlayerLeave onLeave,
                     OnMessage     onMessage,
                     OnReady       onReady,
                     OnError       onError)
    {
        auto transport = std::make_unique<WsTransport>(
            WS_URL,
            onMessage, onJoin, onLeave, onReady, onError
        );
        m_host = std::make_unique<SessionHost>(std::move(transport));
        return m_host->open();
    }

    void closeSession() {
        if (m_host) { m_host->close(); m_host.reset(); }
    }

    std::string getSessionCode() const {
        return m_host ? m_host->getSessionCode() : "";
    }

    bool joinSession(const std::string& code,
                     const std::string& playerName,
                     const std::string& charHash,
                     OnMessage onMessage,
                     OnReady   onReady,
                     OnError   onError)
    {
        auto transport = std::make_unique<WsTransport>(
            WS_URL,
            onMessage,
            nullptr,
            nullptr,
            onReady,
            onError
        );
        m_client = std::make_unique<SessionClient>(
            std::move(transport), playerName, charHash
        );
        return m_client->join(code);
    }

    void leaveSession() {
        if (m_client) { m_client->close(); m_client.reset(); }
    }

    void broadcast(const NetMessage& msg) {
        if (m_host)   m_host->broadcast(msg);
        if (m_client) m_client->send(msg);
    }

    void sendTo(const std::string& playerId, const NetMessage& msg) {
        if (m_host) m_host->sendTo(playerId, msg);
    }

    void publishAsset(const std::vector<uint8_t>& fileData, const std::string& extension = ".webp") {
        if (!isHosting()) return;

        std::string hash = m_cache.saveAsset(fileData, extension);

        std::thread([this, hash, fileData]() {
            auto* ws = dynamic_cast<WsTransport*>(m_host->getTransport());
            
            if (ws && ws->uploadAsset(hash, fileData)) {
                NetMessage msg;
                msg.type = MessageType::AssetAvailable;
                msg.payload = { {"hash", hash} };
                broadcast(msg);
            }
        }).detach();
    }

    void fetchAssetAsync(const std::string& hash) {
        std::thread([this, hash]() {
            std::vector<uint8_t> data;
            
            if (m_cache.loadAsset(hash, data)) {
                std::lock_guard<std::mutex> lock(m_assetMutex);
                m_readyAssets.push({hash, data});
                return;
            }

            INetTransport* transport = isHosting() ? m_host->getTransport() : m_client->getTransport();
            auto* ws = dynamic_cast<WsTransport*>(transport);
            
            if (ws && ws->downloadAsset(hash, data)) {
                m_cache.saveAsset(data, ".webp"); 
                
                std::lock_guard<std::mutex> lock(m_assetMutex);
                m_readyAssets.push({hash, data});
            }
        }).detach();
    }

    void poll() {
        if (m_host)   m_host->poll();
        if (m_client) m_client->poll();

        std::lock_guard<std::mutex> lock(m_assetMutex);
        while (!m_readyAssets.empty()) {
            auto& item = m_readyAssets.front();
            if (m_onAssetReady) {
                m_onAssetReady(item.first, item.second);
            }
            m_readyAssets.pop();
        }
    }

    bool isHosting()   const { return m_host   && m_host->isOpen(); }
    bool isConnected() const { return m_client && m_client->isConnected(); }
    bool isOnline()    const { return isHosting() || isConnected(); }

    SessionHost*   getHost()   { return m_host.get(); }
    SessionClient* getClient() { return m_client.get(); }
};