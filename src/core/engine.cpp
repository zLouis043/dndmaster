#include "engine.h"
#include "../app/views/ViewMainMenu.h"
#include <iostream>

#include <glad/glad.h>
#include <filesystem>

#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/core/SkColorSpace.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "../ui/framework/ElementSkiaCanvas.h"
#include "../ui/framework/ElementColorPicker.h"
#include "./input/InputSystem.h"
#include "./events/Events.h"

namespace fs = std::filesystem;

Engine::Engine() : m_window("DnDMaster", 1280, 720) {}

Engine::~Engine() {
    if (rmlContext) {
        Rml::RemoveContext(rmlContext->GetName());
    }
    Rml::Shutdown();

    skiaSurface.reset();
    skiaContext.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    m_window.destroy();
}

bool Engine::init() {
    
    if (!m_window.init()) return false;

    auto interface = GrGLMakeNativeInterface();
    skiaContext = GrDirectContexts::MakeGL(interface);
    int w, h;
    SDL_GetWindowSizeInPixels(m_window.getNativeWindow(), &w, &h);
    updateSkiaSurface(w, h);

    std::string base = SDL_GetBasePath();
    if (fs::exists(base + "assets/")) {
        m_assetPath = base + "assets/";
    } else if (fs::exists(base + "../../assets/")) {
        m_assetPath = base + "../../assets/";
    } else {
        m_assetPath = "./assets/"; 
    }
    std::cout << "[ENGINE] Asset Path: " << m_assetPath << std::endl;

    Rml::SetSystemInterface(&rmlSystem);
    Rml::SetRenderInterface(&rmlRenderer);
    
    if (Rml::Initialise()) {

        m_canvasInstancer = std::make_unique<InstancerSkiaCanvas>(this);
        Rml::Factory::RegisterElementInstancer("skia-canvas", m_canvasInstancer.get());

        m_colorPickerInstancer = std::make_unique<ColorPickerInstancer>();
        Rml::Factory::RegisterElementInstancer("color-picker", m_colorPickerInstancer.get());

        std::string base_font_path = getAssetPath() + "fonts/Roboto-Regular.ttf";
        if(!Rml::LoadFontFace(base_font_path)){
            fprintf(stderr, "Error loading font\n");
        }
        
        rmlContext = Rml::CreateContext("main", Rml::Vector2i(w, h));
        rmlContext->SetDensityIndependentPixelRatio(1.5f);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(m_window.getNativeWindow(), m_window.getGLContext());
    ImGui_ImplOpenGL3_Init("#version 330 core");

    setupBindings();
    m_window.show();

    return true;
}

void Engine::updateSkiaSurface(int width, int height) {
    if (!skiaContext) return;

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0; 
    framebufferInfo.fFormat = GL_RGBA8;

    auto backendRenderTarget = GrBackendRenderTargets::MakeGL(width, height, 1, 8, framebufferInfo);

    skiaSurface = SkSurfaces::WrapBackendRenderTarget(
        skiaContext.get(), backendRenderTarget,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB(), nullptr
    );

    if (rmlContext) {
        rmlContext->SetDimensions(Rml::Vector2i(width, height));
    }
}

void Engine::setupBindings() {

    m_events.subscribe<MouseMoveEvent>([this](const MouseMoveEvent& e) {
        if (!rmlContext) return false;
        return !rmlContext->ProcessMouseMove(e.x, e.y, 0);
    }, EventDispatcher::Scope::Global);

    m_events.subscribe<MouseButtonEvent>([this](const MouseButtonEvent& e) {
        if (!rmlContext) return false;
        int btnIndex = (int)e.button - 1;
        if (e.pressed) return !rmlContext->ProcessMouseButtonDown(btnIndex, 0);
        else return !rmlContext->ProcessMouseButtonUp(btnIndex, 0);
    }, EventDispatcher::Scope::Global);

    m_events.subscribe<MouseWheelEvent>([this](const MouseWheelEvent& e) {
        if (!rmlContext) return false;
        return !rmlContext->ProcessMouseWheel(-e.delta, 0);
    }, EventDispatcher::Scope::Global);

    m_events.subscribe<KeyEvent>([this](const KeyEvent& e) {

        if (m_shortcuts.processEvent(e)) return false;

        if (!rmlContext) return false;

        auto rmlKey = InputMapper::InternalToRml(e.key);
        int rmlMods = InputMapper::InternalModsToRml(e.mods);

        if (e.pressed) return !rmlContext->ProcessKeyDown(rmlKey, rmlMods);
        else return !rmlContext->ProcessKeyUp(rmlKey, rmlMods);
    }, EventDispatcher::Scope::Global);

    m_events.subscribe<TextInputEvent>([this](const TextInputEvent& e) {
        if (!rmlContext) return false;
        return !rmlContext->ProcessTextInput(e.text);
    }, EventDispatcher::Scope::Global);

    m_events.subscribe<QuitEvent>([this](const QuitEvent&) { quit(); return true; }, EventDispatcher::Scope::Global);

    m_events.subscribe<WindowResizeEvent>([this](const WindowResizeEvent& e) {
        updateSkiaSurface(e.width, e.height);
        return true;
    }, EventDispatcher::Scope::Global);

    m_shortcuts.addGlobal(KeyCode::Z, Mod::Ctrl, [this]() {
        m_commands.undo();
    });
    
    m_shortcuts.addGlobal(KeyCode::Y, Mod::Ctrl, [this]() {
        m_commands.redo();
    });
}

SkCanvas* Engine::getCanvas() const {
    return skiaSurface ? skiaSurface->getCanvas() : nullptr;
}

bool Engine::isPointerOverBlockingUI() {
    auto ctx = getUIContext(); 
    if (!ctx) return false;

    Rml::Element* currentEl = ctx->GetHoverElement();

    while (currentEl) {
        if (currentEl->HasAttribute("data-block")) {
            return true; 
        }
        currentEl = currentEl->GetParentNode();
    }

    return false;
}

void Engine::quit() { isRunning = false; }


void Engine::run() {
    isRunning = true;
    SDL_Event event;

    changeView<ViewMainMenu>();
    
    Uint64 lastTime = SDL_GetTicks();

    SDL_StartTextInput(m_window.getNativeWindow());

    while (isRunning) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (nextView) {
            if (currentView) currentView->exit();

            m_commands.clear();
            m_events.clearLocal();
            m_shortcuts.clearLocal();

            if (rmlContext) {
                rmlContext->Update(); 
            }

            currentView = std::move(nextView);
            currentView->enter(this);
        }

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            InputSystem::processSDLBytEvent(event, m_events);
        }

        if (skiaContext) skiaContext->resetContext();

        SkCanvas* canvas = getCanvas();
        if (canvas) {
            canvas->clear(SkColorSetARGB(255, 20, 40, 60));
        }

        if (rmlContext) rmlContext->Update(); 
        if (rmlContext && canvas) {
            rmlRenderer.SetCanvas(canvas);
            rmlContext->Render();
        }

        if (skiaContext) skiaContext->flush();

        if (true) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            if (currentView) currentView->update(deltaTime);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        m_window.swap();
    }
    
    if (currentView) {
        currentView->exit();
        currentView.reset();
    }
}