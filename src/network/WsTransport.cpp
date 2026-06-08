#include "WsTransport.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace WsMsgType {
    constexpr auto CONNECTED     = "connected";
    constexpr auto CREATE_ROOM   = "create_room";
    constexpr auto ROOM_CREATED  = "room_created";
    constexpr auto JOIN_ROOM     = "join_room";
    constexpr auto JOIN_REQUEST  = "join_request";
    constexpr auto JOIN_ACCEPTED = "join_accepted";
    constexpr auto JOIN_REJECTED = "join_rejected";
    constexpr auto JOIN_SUCCESS = "join_success";
    constexpr auto PLAYER_JOINED = "player_joined";
    constexpr auto PLAYER_LEFT   = "player_left";
    constexpr auto ROOM_CLOSED   = "room_closed";
    constexpr auto BROADCAST     = "broadcast";
    constexpr auto SEND_TO       = "send_to";
    constexpr auto ASSET_AVAILABLE = "asset_available";
}

WsTransport::WsTransport(
    const std::string&               serverUrl,
    INetTransport::OnMessageReceived onMessage,
    INetTransport::OnPlayerJoin      onJoin,
    INetTransport::OnPlayerLeave     onLeave,
    INetTransport::OnSessionReady    onReady,
    INetTransport::OnError           onError)
    : m_onMessage(std::move(onMessage))
    , m_onJoin(std::move(onJoin))
    , m_onLeave(std::move(onLeave))
    , m_onReady(std::move(onReady))
    , m_onError(std::move(onError))
{
    m_serverUrl = serverUrl;
    m_httpUrl   = serverUrl;

    if (m_httpUrl.find("wss://") == 0)
        m_httpUrl.replace(0, 6, "https://");
    else if (m_httpUrl.find("ws://") == 0)
        m_httpUrl.replace(0, 5, "http://");
}

void WsTransport::connectWs() {
    m_ws.setUrl(m_serverUrl);

    m_ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        onWsMessage(msg);
    });

    m_ws.enableAutomaticReconnection();
    m_ws.setMinWaitBetweenReconnectionRetries(2000);
    m_ws.start();
}

bool WsTransport::openSession() {
    m_isHost = true;
    connectWs();

    int waited = 0;
    while (!m_connected && waited < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waited++;
    }

    if (!m_connected) {
        if (m_onError) m_onError("Impossibile connettersi al server");
        return false;
    }

    sendJson({ {"type", WsMsgType::CREATE_ROOM} });

    waited = 0;
    while (m_sessionCode.empty() && waited < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waited++;
    }

    if (m_sessionCode.empty()) {
        if (m_onError) m_onError("Timeout creazione stanza");
        return false;
    }

    if (m_onReady) m_onReady(m_sessionCode);
    
    return true;
}

bool WsTransport::joinSession(const std::string& code) {
    m_isHost = false;
    m_ready  = false;

    std::string cleanCode = code;
    cleanCode.erase(std::remove_if(cleanCode.begin(), cleanCode.end(), ::isspace), cleanCode.end());
    for (auto& c : cleanCode) c = std::toupper(c);

    m_sessionCode = cleanCode;
    connectWs();

    int waited = 0;
    while (!m_connected && waited < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waited++;
    }

    if (!m_connected) {
        if (m_onError) m_onError("Impossibile connettersi al server");
        return false;
    }

    sendJson({
        {"type", WsMsgType::JOIN_ROOM},
        {"code", m_sessionCode}
    });

    waited = 0;
    while (!m_ready && waited < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waited++;
    }

    if (!m_ready) {
        if (m_onError) m_onError("Timeout ingresso stanza");
        return false;
    }

    return true;
}

void WsTransport::send(const std::string& toId, const NetMessage& msg) {
    sendJson({
        {"type",     WsMsgType::SEND_TO},
        {"targetId", toId},
        {"payload",  msg.serialize()}
    });
}

void WsTransport::broadcast(const NetMessage& msg) {
    sendJson({
        {"type",    WsMsgType::BROADCAST},
        {"payload", msg.serialize()}
    });
}

void WsTransport::poll() {
    std::queue<NetMessage> toProcess;
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        std::swap(toProcess, m_incomingQueue);
    }

    while (!toProcess.empty()) {
        const NetMessage& msg = toProcess.front();

        switch (msg.type) {
            case MessageType::JoinRequest:
                if (m_isHost && m_onJoin) {
                    std::string pid  = msg.senderId;
                    std::string hash = msg.payload.value("hash", "");
                    bool accepted    = m_onJoin(pid, hash);

                    sendJson({
                        {"type",     accepted ? WsMsgType::JOIN_ACCEPTED 
                                              : WsMsgType::JOIN_REJECTED},
                        {"targetId", pid},
                        {"reason",   accepted ? "" : "Hash non valido"}
                    });
                }
                break;

            case MessageType::JoinAccepted:
                if (!m_isHost) {
                    if (m_onReady) m_onReady(m_sessionCode);
                }
                break;

            case MessageType::JoinRejected:
                if (!m_isHost) {
                    std::string reason = msg.payload.value("reason", "Accesso negato");
                    if (m_onError) m_onError(reason);
                }
                break;

            case MessageType::PlayerLeft:
                if (m_onLeave)
                    m_onLeave(msg.payload.value("id", ""));
                break;

            default:
                if (m_onMessage) m_onMessage(msg);
                break;
        }

        toProcess.pop();
    }
}

void WsTransport::close() {
    m_connected = false;
    m_ws.stop();
}

void WsTransport::onWsMessage(const ix::WebSocketMessagePtr& msg) {
    switch (msg->type) {
        case ix::WebSocketMessageType::Message:
            std::cout << "[WS] Messaggio ricevuto: " << msg->str << std::endl;
            handleJson(msg->str);
            break;

        case ix::WebSocketMessageType::Open:
            std::cout << "[WS] Connessione aperta" << std::endl;
            m_connected = true;
            break;

        case ix::WebSocketMessageType::Close:
            std::cout << "[WS] Connessione chiusa: " << msg->closeInfo.reason << std::endl;
            m_connected = false;
            break;

        case ix::WebSocketMessageType::Error:
            std::cout << "[WS] Errore: " << msg->errorInfo.reason << std::endl;
            if (m_onError) m_onError(msg->errorInfo.reason);
            break;

        default:
            break;
    }
}

void WsTransport::handleJson(const std::string& raw) {
    try {
        auto root = nlohmann::json::parse(raw);
        std::string type = root.value("type", "");

        if (type == WsMsgType::CONNECTED) {
            m_selfId = root.value("clientId", "");
            return;
        }

        if (type == WsMsgType::ROOM_CREATED) {
            m_sessionCode = root.value("code", "");
            if (m_onReady) m_onReady(m_sessionCode);
            return;
        }

        if (type == WsMsgType::JOIN_REQUEST) {
            return;
        }

        if (type == WsMsgType::JOIN_ACCEPTED) {
            NetMessage msg;
            msg.type     = MessageType::JoinAccepted;
            msg.senderId = "server";
            msg.payload  = { {"id", root.value("clientId", "")} };
            pushIncoming(msg);
            return;
        }

        if (type == WsMsgType::JOIN_REJECTED) {
            NetMessage msg;
            msg.type     = MessageType::JoinRejected;
            msg.senderId = "server";
            msg.payload  = { {"reason", root.value("reason", "")} };
            pushIncoming(msg);
            return;
        }

        if (type == WsMsgType::JOIN_SUCCESS) {
            m_ready = true;
            return;
        }

        if (type == WsMsgType::PLAYER_LEFT) {
            NetMessage msg;
            msg.type     = MessageType::PlayerLeft;
            msg.senderId = "server";
            msg.payload  = { {"id", root.value("clientId", "")} };
            pushIncoming(msg);
            return;
        }

        if (type == WsMsgType::ROOM_CLOSED) {
            NetMessage msg;
            msg.type     = MessageType::SessionClosed;
            msg.senderId = "server";
            pushIncoming(msg);
            return;
        }

        if (type == WsMsgType::BROADCAST || type == WsMsgType::SEND_TO) {
            std::string payload = root.value("payload", "");
            if (!payload.empty()) {
                NetMessage msg = NetMessage::deserialize(payload);
                msg.senderId = root.value("senderId", "");
                
                pushIncoming(msg);
            }
            return;
        }

    } catch (const std::exception& e) {
        std::cerr << "[WS] Parse error: " << e.what() << std::endl;
    }
}

void WsTransport::sendJson(const nlohmann::json& obj) {
    if (!m_connected) return;
    m_ws.send(obj.dump());
}

void WsTransport::pushIncoming(const NetMessage& msg) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_incomingQueue.push(msg);
}

bool WsTransport::uploadAsset(const std::string& hash,
                               const std::vector<uint8_t>& data,
                               const std::string& mimeType) {
    ix::HttpClient http;

    auto args = std::make_shared<ix::HttpRequestArgs>();
    args->extraHeaders = {
        {"Content-Type", mimeType},
        {"X-Room-Code",  m_sessionCode},
        {"X-Client-Id",  m_selfId}
    };

    std::string body(data.begin(), data.end());
    auto res = http.post(m_httpUrl + "/asset/" + hash, body, args);
    return res->statusCode == 200 || res->statusCode == 201;
}

bool WsTransport::downloadAsset(const std::string& hash,
                                 std::vector<uint8_t>& outData) {
    ix::HttpClient http;
    
    auto args = std::make_shared<ix::HttpRequestArgs>();
    auto res  = http.get(m_httpUrl + "/asset/" + hash, args);

    if (res->statusCode != 200) return false;
    outData.assign(res->body.begin(), res->body.end());
    return true;
}