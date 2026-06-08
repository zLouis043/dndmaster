#pragma once
#include "WsTransport.h"
#include <unordered_map>
#include <string>

struct ConnectedPlayer {
    std::string id;
    std::string name;
    std::string characterHash;
};

class SessionHost {
    std::unique_ptr<INetTransport>                   m_transport;
    std::unordered_map<std::string, ConnectedPlayer> m_players;

public:
    explicit SessionHost(std::unique_ptr<INetTransport> transport)
        : m_transport(std::move(transport)) {}

    bool open()  { return m_transport->openSession(); }
    void poll()  { m_transport->poll(); }
    void close() { m_transport->close(); }

    std::string getSessionCode() const { return m_transport->getSessionCode(); }
    bool        isOpen()         const { return m_transport->isConnected(); }

    void sendTo(const std::string& playerId, const NetMessage& msg) {
        m_transport->send(playerId, msg);
    }
    void broadcast(const NetMessage& msg) {
        m_transport->broadcast(msg);
    }

    void addPlayer(const ConnectedPlayer& p)         { m_players[p.id] = p; }
    void removePlayer(const std::string& id)         { m_players.erase(id); }
    bool hasPlayer(const std::string& id)      const { return m_players.count(id) > 0; }

    const std::unordered_map<std::string, ConnectedPlayer>& getPlayers() const {
        return m_players;
    }

    INetTransport* getTransport() { return m_transport.get(); }
};