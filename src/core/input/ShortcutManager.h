#pragma once
#include "InputCodes.h"
#include "../events/Events.h"
#include <functional>
#include <vector>
#include <algorithm>

class ShortcutManager {
public:
    enum class Scope { Global, Local };

    struct Shortcut {
        KeyCode key;
        KeyModifiers mods;
        Scope scope;
        std::function<void()> action;

        bool matches(const KeyEvent& e) const {
            return e.pressed && 
                   e.key == key && 
                   e.mods.ctrl == mods.ctrl && 
                   e.mods.shift == mods.shift && 
                   e.mods.alt == mods.alt;
        }
    };

    void addGlobal(KeyCode key, KeyModifiers mods, std::function<void()> action) {
        m_shortcuts.push_back({key, mods, Scope::Global, std::move(action)});
    }

    void addLocal(KeyCode key, KeyModifiers mods, std::function<void()> action) {
        m_shortcuts.push_back({key, mods, Scope::Local, std::move(action)});
    }

    void clearLocal() {
        m_shortcuts.erase(
            std::remove_if(m_shortcuts.begin(), m_shortcuts.end(),
                [](const Shortcut& s) { return s.scope == Scope::Local; }),
            m_shortcuts.end()
        );
    }

    bool processEvent(const KeyEvent& e) {
        for (const auto& s : m_shortcuts) {
            if (s.matches(e)) {
                s.action();
                return true; 
            }
        }
        return false;
    }

private:
    std::vector<Shortcut> m_shortcuts;
};