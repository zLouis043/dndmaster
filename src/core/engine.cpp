#include "engine.h"
#include "../app/ViewMainMenu.h"
#include <iostream>

// Backend OpenGL
#include <glad/glad.h>

// Include Skia (API Aggiornata v146+ / Ganesh)
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/core/SkColorSpace.h>

// Include ImGui
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

Engine::Engine() {}

Engine::~Engine() {
    // Ordine di distruzione vitale! Prima la superficie, poi il contesto Skia
    skiaSurface.reset();
    skiaContext.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (gl_context) SDL_GL_DestroyContext(gl_context);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Engine::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    // Richiediamo a SDL un contesto OpenGL compatibile con Skia (Core Profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // Importante per Skia

    window = SDL_CreateWindow("DnDMaster - Skia Integration", 1280, 720, 
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // VSync
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return false;

    // --- SETUP SKIA (Sintassi Aggiornata) ---
    auto interface = GrGLMakeNativeInterface();
    // In Skia 146 usiamo GrDirectContexts (plurale) e passiamo da gl/
    skiaContext = GrDirectContexts::MakeGL(interface);
    if (!skiaContext) {
        std::cerr << "Errore creazione contesto Skia!\n";
        return false;
    }

    // Creiamo la superficie di disegno iniziale
    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    updateSkiaSurface(w, h);

    SDL_ShowWindow(window);

    // --- SETUP IMGUI ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    
    
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return true;
}

void Engine::updateSkiaSurface(int width, int height) {
    if (!skiaContext) return;

    // Definiamo il target di rendering (il Framebuffer di default di OpenGL, che è 0)
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0; 
    framebufferInfo.fFormat = GL_RGBA8;

    // Sintassi Skia 146: GrBackendRenderTargets (plurale)
    auto backendRenderTarget = GrBackendRenderTargets::MakeGL(width, height, 1, 8, framebufferInfo);

    // Sintassi Skia 146: SkSurfaces (plurale)
    skiaSurface = SkSurfaces::WrapBackendRenderTarget(
        skiaContext.get(),
        backendRenderTarget,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        SkColorSpace::MakeSRGB(),
        nullptr
    );
}

SkCanvas* Engine::getCanvas() const {
    return skiaSurface ? skiaSurface->getCanvas() : nullptr;
}

void Engine::quit() { isRunning = false; }
void Engine::changeView(std::unique_ptr<IAppView> newView) { nextView = std::move(newView); }

void Engine::run() {
    isRunning = true;
    SDL_Event event;

    changeView(std::make_unique<ViewMainMenu>());
    Uint64 lastTime = SDL_GetTicks();

    while (isRunning) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (nextView) {
            if (currentView) currentView->onExit(this);
            currentView = std::move(nextView);
            currentView->onEnter(this);
        }

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) quit();
            
            // Gestione del Resize per Skia!
            if (event.type == SDL_EVENT_WINDOW_RESIZED || event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                int w, h;
                SDL_GetWindowSizeInPixels(window, &w, &h);
                updateSkiaSurface(w, h);
            }
        }

        // 1. Pulisci lo schermo (con Skia)
        SkCanvas* canvas = getCanvas();
        if (canvas) {
            canvas->clear(SkColorSetARGB(255, 30, 30, 30)); // Sfondo grigio scuro
        }

        // 2. Prepara ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        // 3. Esegui la logica della View (che userà sia SkCanvas che ImGui)
        if (currentView) {
            currentView->onUpdate(this, deltaTime);
        }

        // 4. Flush di Skia (forza il rendering di tutto quello che hai disegnato finora)
        if (skiaContext) {
            skiaContext->flush();
        }

        // 5. Renderizza ImGui *sopra* Skia
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 6. Swap!
        SDL_GL_SwapWindow(window);
    }
    
    if (currentView) {
        currentView->onExit(this);
        currentView.reset();
    }
}