#pragma once
#include <functional>
#include <vector>
#include <typeindex>
#include <map>
#include <memory>

class IEvent {
public:
    virtual ~IEvent() = default;
};

class EventDispatcher {
public:

    enum class Scope { Global, Local };

    template<typename T>
    using EventCallback = std::function<bool(const T&)>;

    template<typename T>
    void subscribe(std::function<bool(const T&)> callback, Scope scope = Scope::Local) {
        m_subscribers[typeid(T)].push_back({scope, [callback](const IEvent& e) -> bool {
            return callback(static_cast<const T&>(e));
        }});
    }

    template<typename T>
    void dispatch(const T& event) {
        auto it = m_subscribers.find(typeid(T));
        if (it != m_subscribers.end()) {
            for (auto& listener : it->second) {
                if(listener.callback(event)) return;
            }
        }
    }

    void clearLocal() {
        for (auto& pair : m_subscribers) {
            auto& list = pair.second;
            list.erase(std::remove_if(list.begin(), list.end(), [](const Listener& l) {
                return l.scope == Scope::Local;
            }), list.end());
        }
    }

private:
    struct Listener {
        Scope scope;
        std::function<bool(const IEvent&)> callback;
    };
    std::map<std::type_index, std::vector<Listener>> m_subscribers;
};