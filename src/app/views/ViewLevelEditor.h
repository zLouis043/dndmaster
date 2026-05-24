#pragma once
#include "../../ui/framework/IAppView.h"
#include "../../core/input/InputCodes.h"
#include "../../data/entities/MapEntity.h"
#include "../tools/ToolManager.h"
#include <include/core/SkPoint.h>

class ViewLevelEditor : public IAppView {
public:
    ViewLevelEditor(int mapId) : IAppView("pages/level_editor.rml"), targetMapId(mapId) {}

protected:
    void onEnter() override;
    void onUpdate(float deltaTime) override {}
    void onExit() override;

private:
    int targetMapId;
    MapEntity currentMap;
    ToolManager m_toolManager;

    // Trasformazioni Telecamera
    float zoom = 1.0f;
    float panX = 0.0f, panY = 0.0f;
    bool isPanning = false;
    float mouseX = 0.0f, mouseY = 0.0f;

    // Metodi privati interni
    void loadFromDb();
    void saveToDb();
    void updateSettingsPanel();
    void handleCanvasDraw(class SkCanvas* canvas, float width, float height);
    SkPoint screenToWorld(SkPoint screenPos);
};