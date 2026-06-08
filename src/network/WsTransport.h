#pragma once
#include "INetTransport.h"
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXHttpClient.h>
#include <atomic>
#include <mutex>
#include <queue>
#include <string>

class WsTransport : public INetTransport {
private:
    std::string m_serverUrl;
    std::string m_httpUrl;
    std::string m_selfId;
    std::string m_sessionCode;
    bool        m_isHost = false;

    ix::WebSocket m_ws;

    INetTransport::OnMessageReceived m_onMessage;
    INetTransport::OnPlayerJoin      m_onJoin;
    INetTransport::OnPlayerLeave     m_onLeave;
    INetTransport::OnSessionReady    m_onReady;
    INetTransport::OnError           m_onError;

    std::mutex             m_queueMutex;
    std::queue<NetMessage> m_incomingQueue;

    std::atomic<bool> m_connected { false };
    std::atomic<bool> m_ready     { false };

public:
    WsTransport(
        const std::string&               serverUrl,
        INetTransport::OnMessageReceived onMessage,
        INetTransport::OnPlayerJoin      onJoin,
        INetTransport::OnPlayerLeave     onLeave,
        INetTransport::OnSessionReady    onReady,
        INetTransport::OnError           onError
    );

    ~WsTransport() override { close(); }

    bool        openSession()                        override;
    bool        joinSession(const std::string& code) override;
    void        send(const std::string& toId,
                     const NetMessage& msg)          override;
    void        broadcast(const NetMessage& msg)     override;
    void        poll()                               override;
    void        close()                              override;
    std::string getSessionCode()               const override { return m_sessionCode; }
    bool        isConnected()                  const override { return m_connected; }

    bool uploadAsset(const std::string& hash,
                     const std::vector<uint8_t>& data,
                     const std::string& mimeType = "image/webp");

    bool downloadAsset(const std::string& hash,
                       std::vector<uint8_t>& outData);

private:
    void connectWs();
    void onWsMessage(const ix::WebSocketMessagePtr& msg);
    void handleJson(const std::string& raw);
    void pushIncoming(const NetMessage& msg);

    void sendJson(const nlohmann::json& obj);
};