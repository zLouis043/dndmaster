#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include "../data/DatabaseEngine.h"

#include <include/core/SkRefCnt.h>

// Forward declarations per Skia
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
    
    void changeView(std::unique_ptr<IAppView> newView);
    DatabaseEngine& getDB() { return dbEngine; };

    // Getter per Skia: la View chiederà il "Canvas" su cui disegnare!
    class SkCanvas* getCanvas() const;

    SDL_Window* getWindow() { return window;}

private:
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    bool isRunning = false;
    
    // Oggetti centrali di Skia
    sk_sp<GrDirectContext> skiaContext;
    sk_sp<SkSurface> skiaSurface;

    std::unique_ptr<IAppView> currentView;
    std::unique_ptr<IAppView> nextView; 

    DatabaseEngine dbEngine;

    // Metodo helper per ricreare la superficie se la finestra viene ridimensionata
    void updateSkiaSurface(int width, int height);
};