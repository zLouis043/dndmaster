#pragma once
#include "IMapTool.h"
#include "../../core/engine.h"
#include "../events/EditorEvents.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>

class ToolManager : public IInspectable {
private:
    struct ToolEntry {
        std::shared_ptr<IMapTool> tool;
        std::function<void()> action;
    };
    std::vector<ToolEntry> m_entries;
    std::shared_ptr<IMapTool> m_activeTool = nullptr;
    Engine* m_engine = nullptr;

public:
    void init(Engine* engine) { 
        m_engine = engine; 
    }

    std::string getType() const override { return "tool_manager"; }

    template <typename T, typename... Args>
    void registerTool(Args&&... args) {
        auto tool = std::make_shared<T>(std::forward<Args>(args)...);
        
        std::function<void()> switchAction = [this, tool]() {
            if (m_activeTool) m_activeTool->onCancel();
            m_activeTool = tool;
            
            if (m_engine) m_engine->getEvents().dispatch(EventToolChanged{});
        };
        
        m_entries.push_back({tool, switchAction});
        if (!m_activeTool) m_activeTool = tool;

        if (m_engine && tool->getShortcut() != KeyCode::Unknown) {
            m_engine->getShortcuts().addLocal(tool->getShortcut(), tool->getShortcutModifier(), switchAction);
        }
    }

    std::shared_ptr<IMapTool> getActiveTool() const { return m_activeTool; }
    
    void inspect(IInspector& inspector) override {
        inspector.name("STRUMENTI");
        for (size_t i = 0; i < m_entries.size(); ++i) {
            bool isActive = (m_activeTool == m_entries[i].tool);
            std::string btnClass = isActive ? "tool-btn active" : "tool-btn";
            
            inspector.property("tool_btn_" + std::to_string(i), m_entries[i].action)
                     .tag("ui-label", m_entries[i].tool->getToolName())
                     .tag("ui-class", btnClass);
        }
    }

    bool onMouseDown(SkPoint pos, Engine* e, MapEntity& m) { return m_activeTool ? m_activeTool->onMouseDown(pos, e, m) : false; }
    bool onMouseMove(SkPoint pos, Engine* e, MapEntity& m) { return m_activeTool ? m_activeTool->onMouseMove(pos, e, m) : false; }
    bool onMouseUp(SkPoint pos, Engine* e, MapEntity& m) { return m_activeTool ? m_activeTool->onMouseUp(pos, e, m) : false; }
    void drawGhost(SkCanvas* c, float z) { if (m_activeTool) m_activeTool->drawGhost(c, z); }

    void cancelActiveTool() {
        if (m_activeTool) m_activeTool->onCancel();
    }

    void setActiveTool(const std::string& typeName) {
        for (auto& entry : m_entries) {
            if (entry.tool->getType() == typeName) {
                entry.action();
                break;
            }
        }
    }
};