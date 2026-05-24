#include "ViewLevelEditor.h"
#include "ViewCampaignHub.h"
#include "../../core/engine.h"
#include "../../core/rendering/MapRenderingEngine.h"

#include "../tools/implementations/ToolDrawWall.h"
#include "../tools/implementations/ToolPlaceToken.h"
#include "../tools/implementations/ToolErase.h"

#include "../../data/assets/ShaderAsset.h"

void ViewLevelEditor::onEnter() {
    loadFromDb();

    getEngine()->getAssets().load<sk_sp<SkRuntimeEffect>>("vtt_grid", "shaders/grid.sksl");

    m_toolManager.init(getEngine());
    m_toolManager.registerTool<ToolDrawWall>();
    m_toolManager.registerTool<ToolPlaceToken>();
    m_toolManager.registerTool<ToolErase>();

    getEngine()->getEvents().subscribe<EventToolChanged>([this](const EventToolChanged&) {
        currentMap.clearSelection(getEngine());
        defer([this]() { updateSettingsPanel(); });
        return false;
    });

    getEngine()->getEvents().subscribe<EventSelectionChanged>([this](const EventSelectionChanged&) {
        defer([this]() { updateSettingsPanel(); });
        return false;
    });

    mount("toolbar_mount", &m_toolManager, "tool_list");
    updateSettingsPanel();

    bindCanvas("map_canvas", [this](SkCanvas* c, float w, float h) { handleCanvasDraw(c, w, h); });

    bindEvent("btn_save", Rml::EventId::Click, [this](Rml::Event&) {
        saveToDb();
        std::cout << "[EDITOR] Mappa Salvata!" << std::endl;
    });
    
    bindEvent("btn_reset", Rml::EventId::Click, [this](Rml::Event&) {
        currentMap.elements.clear();
        getEngine()->getCommands().clear();
    });

    bindEvent("btn_exit", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->changeView<ViewCampaignHub>();
    });

    bindEvent("map_canvas", Rml::EventId::Mousedown, [this](Rml::Event& ev) {
        int button = ev.GetParameter<int>("button", 0);
        if (button == 1 || button == 2) { isPanning = true; return; }
        SkPoint worldPos = screenToWorld(SkPoint::Make(mouseX, mouseY));
        if (button == 0) { if (m_toolManager.onMouseDown(worldPos, getEngine(), currentMap)) saveToDb(); }
    });

    bindEvent("map_canvas", Rml::EventId::Mousemove, [this](Rml::Event& ev) {
        float x = ev.GetParameter<float>("mouse_x", 0.0f);
        float y = ev.GetParameter<float>("mouse_y", 0.0f);
        if (isPanning) { panX += (x - mouseX); panY += (y - mouseY); }
        mouseX = x; mouseY = y;
        SkPoint worldPos = screenToWorld(SkPoint::Make(x, y));
        if (m_toolManager.onMouseMove(worldPos, getEngine(), currentMap)) saveToDb();
    });

    bindEvent("map_canvas", Rml::EventId::Mouseup, [this](Rml::Event& ev) {
        int button = ev.GetParameter<int>("button", 0);
        if (button == 1 || button == 2) { isPanning = false; return; }
        SkPoint worldPos = screenToWorld(SkPoint::Make(mouseX, mouseY));
        if (m_toolManager.onMouseUp(worldPos, getEngine(), currentMap)) saveToDb();
    });

    bindEvent("map_canvas", Rml::EventId::Mousescroll, [this](Rml::Event& ev) {
        float delta = ev.GetParameter<float>("wheel_delta_y", 0.0f);
        if (delta == 0.0f) delta = ev.GetParameter<float>("wheel_delta", 0.0f);

        zoom -= delta * 0.1f; 
        if (zoom < 0.1f) zoom = 0.1f;
        if (zoom > 5.0f) zoom = 5.0f;
    });

    addShortcut(KeyCode::Z, Mod::Ctrl, [this]() { getEngine()->getCommands().undo(); saveToDb(); });
    addShortcut(KeyCode::Y, Mod::Ctrl, [this]() { getEngine()->getCommands().redo(); saveToDb(); });
    addShortcut(KeyCode::Escape, Mod::None, [this]() { getEngine()->changeView<ViewCampaignHub>(); });

    addShortcut(KeyCode::Space, Mod::None, [this]() { m_toolManager.cancelActiveTool(); });
}

void ViewLevelEditor::onExit() {
    unmount("toolbar_mount");
    unmount("settings_mount");
}

void ViewLevelEditor::updateSettingsPanel() {
    mount("toolbar_mount", &m_toolManager, "tool_list");
    
    unmount("settings_mount");
    if (currentMap.selectedElement) {
        mount("settings_mount", currentMap.selectedElement.get(), "tool-settings");
    } else if (auto activeTool = m_toolManager.getActiveTool()) {
        mount("settings_mount", activeTool.get(), "tool-settings");
    }
}

void ViewLevelEditor::saveToDb() {
    if (getEngine()->getDB().save(currentMap)) {
        std::cout << "[EDITOR] Mappa Salvata Correttamente!" << std::endl;
    } else {
        std::cerr << "[EDITOR] Errore durante il salvataggio!" << std::endl;
    }
}

void ViewLevelEditor::loadFromDb() {
    if (getEngine()->getDB().getById<MapEntity>(targetMapId, currentMap)) {
        currentMap.markGridDirty(); 
    }
}

SkPoint ViewLevelEditor::screenToWorld(SkPoint screenPos) {
    return SkPoint::Make((screenPos.x() - panX) / zoom, (screenPos.y() - panY) / zoom);
}

void ViewLevelEditor::handleCanvasDraw(SkCanvas* canvas, float width, float height) {
    canvas->save();

    auto gridEffect = getEngine()->getAssets().get<sk_sp<SkRuntimeEffect>>("vtt_grid");
    if (gridEffect) {
        SkRuntimeShaderBuilder builder(gridEffect);
        builder.uniform("resolution") = SkV2{width, height};
        builder.uniform("color")      = SkV4{0.8f, 0.8f, 0.8f, 0.3f};
        builder.uniform("zoom")       = zoom;
        builder.uniform("pan")        = SkV2{panX, panY};
        builder.uniform("gridStep")   = (float)currentMap.gridSize;

        SkPaint gridPaint;
        gridPaint.setShader(builder.makeShader());
        
        canvas->drawRect(SkRect::MakeWH(width, height), gridPaint);
    }
    canvas->restore(); 

    canvas->save(); 

    canvas->translate(panX, panY);
    canvas->scale(zoom, zoom);

    MapRenderingEngine::render(canvas, currentMap.elements, zoom);
    if (!currentMap.selectedElement) m_toolManager.drawGhost(canvas, zoom);

    canvas->restore();
}