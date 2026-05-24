#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include "../data/db/DatabaseEngine.h"
#include <include/core/SkRefCnt.h>

#include <RmlUi/Core.h>
#include <RmlUi/Core/ElementInstancer.h>
#include "../ui/backend/RmlSystem.h"
#include "../ui/backend/RmlSkiaRenderer.h"

#include "./events/EventDispatcher.h"
#include "./window/window.h"
#include "./commands/CommandManager.h"
#include "./input/ShortcutManager.h"
#include "./assets/AssetManager.h"

class GrDirectContext;
class SkSurface;

class IAppView;

class Engine {
public:
    Engine();
    ~Engine();

    bool init();
    void run();
    void quit();
    
    template <typename View, typename... Args>
    void changeView(Args... args) { nextView = std::make_unique<View>(std::forward<Args>(args)...); }
    DatabaseEngine& getDB() { return dbEngine; };

    class SkCanvas* getCanvas() const;
    Window getWindow() { return m_window; }

    Rml::Context* getUIContext() { return rmlContext; }
    EventDispatcher& getEvents() { return m_events; }
    CommandManager& getCommands() { return m_commands; }
    ShortcutManager& getShortcuts() { return m_shortcuts; }
    AssetManager& getAssets() { return m_assets; }
    const std::string& getAssetPath() const { return m_assetPath; }

    bool isPointerOverBlockingUI();

private:
    Window m_window;
    EventDispatcher m_events;
    ShortcutManager m_shortcuts;
    CommandManager m_commands;
    AssetManager m_assets;

    bool isRunning = false;
    
    sk_sp<GrDirectContext> skiaContext;
    sk_sp<SkSurface> skiaSurface;

    RmlSystem rmlSystem;
    RmlSkiaRenderer rmlRenderer;
    Rml::Context* rmlContext = nullptr;
    
    std::unique_ptr<IAppView> currentView;
    std::unique_ptr<IAppView> nextView; 

    DatabaseEngine dbEngine;
    std::string m_assetPath;

    std::unique_ptr<Rml::ElementInstancer> m_canvasInstancer;
    std::unique_ptr<Rml::ElementInstancer> m_colorPickerInstancer;

    void updateSkiaSurface(int width, int height);
    void setupBindings();
};