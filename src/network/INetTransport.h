#pragma once
#include "NetMessage.h"
#include <functional>
#include <string>

class INetTransport {
public:
    using OnMessageReceived = std::function<void(const NetMessage&)>;
    using OnPlayerJoin      = std::function<bool(const std::string& playerId,
                                                  const std::string& hash)>;
    using OnPlayerLeave     = std::function<void(const std::string& playerId)>;
    using OnSessionReady    = std::function<void(const std::string& code)>;
    using OnError           = std::function<void(const std::string& reason)>;

    virtual ~INetTransport() = default;

    virtual bool        openSession()                              = 0;
    virtual bool        joinSession(const std::string& code)       = 0;
    virtual void        send(const std::string& toId,
                             const NetMessage& msg)                = 0;
    virtual void        broadcast(const NetMessage& msg)           = 0;
    virtual void        poll()                                     = 0;
    virtual void        close()                                    = 0;
    virtual std::string getSessionCode()                     const = 0;
    virtual bool        isConnected()                        const = 0;
};