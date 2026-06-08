#pragma once
#include "../data/inspector/Inspection.h"
#include "../data/plugs/JsonPlugs.h"
#include <nlohmann/json.hpp>
#include <cstdint>
#include <string>

enum class MessageType : uint8_t {
    JoinRequest   = 1,
    JoinAccepted  = 2,
    JoinRejected  = 3,
    PlayerJoined  = 4,
    PlayerLeft    = 5,
    SessionClosed = 6,

    StateUpdate   = 11,

    GameEvent     = 21, 
    AssetAvailable = 30,
};

struct NetMessage : public IInspectable {
    MessageType    type     = MessageType::JoinRequest;
    std::string    senderId;
    nlohmann::json payload;

    std::string getType() const override { return "NetMessage"; }

    void inspect(IInspector& ins) override {
    int t = static_cast<int>(type);
    ins.property("type", t);
    if (ins.isReading()) type = static_cast<MessageType>(t);
    ins.property("sender", senderId);
    ins.property("payload", payload).tag("ui-ignore", true);
}

    std::string serialize() const {
        nlohmann::json root;
        JsonWriter writer(root);
        const_cast<NetMessage*>(this)->inspect(writer);
        return root.dump();
    }

    static NetMessage deserialize(const std::string& raw) {
        try {
            nlohmann::json root = nlohmann::json::parse(raw);
            JsonReader reader(root);
            NetMessage msg;
            msg.inspect(reader);
            return msg;
        } catch (...) {
            return NetMessage{};
        }
    }
};