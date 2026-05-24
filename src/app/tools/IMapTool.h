#pragma once
#include "../../data/inspector/Inspection.h"
#include <include/core/SkCanvas.h>
#include <include/core/SkPoint.h>
#include <string>

class Engine;
class MapEntity;

class IMapTool : public IInspectable {
public:
    virtual ~IMapTool() = default;
    virtual std::string getToolName() const = 0;

    virtual KeyCode getShortcut() const { return KeyCode::Unknown; }
    virtual KeyModifiers getShortcutModifier() const { return Mod::None; }

    virtual bool onMouseDown(SkPoint worldPos, Engine* engine, MapEntity& map) { return false; }
    virtual bool onMouseMove(SkPoint worldPos, Engine* engine, MapEntity& map) { return false; }
    virtual bool onMouseUp(SkPoint worldPos, Engine* engine, MapEntity& map) { return false; }

    virtual void drawGhost(SkCanvas* canvas, float zoom) {}
    virtual void onCancel() {}
};