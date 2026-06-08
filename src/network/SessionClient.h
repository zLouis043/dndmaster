#pragma once
#include "WsTransport.h"
#include <string>

class SessionClient {
    std::unique_ptr<INetTransport> m_transport;
    std::string                    m_name;
    std::string                    m_charHash;

public:
    explicit SessionClient(std::unique_ptr<INetTransport> transport,
                           std::string name,
                           std::string charHash)
        : m_transport(std::move(transport))
        , m_name(std::move(name))
        , m_charHash(std::move(charHash)) {}

    bool join(const std::string& code) {
        if (!m_transport->joinSession(code)) return false;

        NetMessage req;
        req.type    = MessageType::JoinRequest;
        req.payload = {
            {"name", m_name},
            {"hash", m_charHash}
        };
        m_transport->broadcast(req);
        return true;
    }

    void poll()  { m_transport->poll(); }
    void close() { m_transport->close(); }
    void send(const NetMessage& msg) { m_transport->broadcast(msg); }

    bool isConnected() const { return m_transport->isConnected(); }
    INetTransport* getTransport() { return m_transport.get(); }
};