#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <RmlUi/Core.h>
#include "LambdaListener.h"
#include "ElementSkiaCanvas.h"
#include "../../core/input/InputCodes.h"

class Engine;
class IInspectable;

class IAppView {
public:
    IAppView(const std::string& rmlPath) : m_rmlPath(rmlPath) {}
    virtual ~IAppView() = default;

    void enter(Engine* engine);
    void update(float deltaTime);
    void exit();

protected:
    virtual void onEnter() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onExit() {} 

    void bindEvent(const std::string& element_id, Rml::EventId event_id, std::function<void(Rml::Event&)> callback);
    void bindCanvas(const std::string& element_id, std::function<void(class SkCanvas*, float, float)> drawCallback);
    void addShortcut(KeyCode key, KeyModifiers mods, std::function<void()> action);
    
    void defer(std::function<void()> action) { m_deferredActions.push_back(std::move(action)); }

    void mount(const std::string& mountId, IInspectable* obj, const std::string& defaultContainerId, const std::vector<std::string>& relatedTargets = {});
    void unmount(const std::string& mountId);
    void readUI(IInspectable* obj, const std::string& defaultContainerId);
    void writeUI(IInspectable* obj, const std::string& defaultContainerId);

public:
    Engine* getEngine() { return m_engine; };
    Rml::ElementDocument* getDocument() { return document; };

private:
    void clearUI(const std::vector<std::string>& containerIds);
    void buildUI(IInspectable* obj, const std::string& defaultContainerId);

    Engine* m_engine = nullptr;
    Rml::ElementDocument* document = nullptr;
    std::string m_rmlPath;
    
    std::vector<std::function<void()>> m_deferredActions;

    struct UIMount {
        IInspectable* obj;
        std::string defaultContainerId;
        std::vector<std::string> relatedTargets;
    };
    std::unordered_map<std::string, UIMount> m_mounts;
};