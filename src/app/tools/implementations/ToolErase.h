#pragma once
#include "../IMapTool.h"
#include "../../commands/EditorCommands.h"
#include "../../../core/physics/CollisionEngine.h"
#include "../../../data/entities/MapEntity.h"
#include <include/core/SkPaint.h>

class ToolErase : public IMapTool {
public:
    float radius = 25.0f;
    SkPoint lastMousePos = SkPoint::Make(0,0);

    std::string getType() const override { return "tool_erase"; }
    std::string getToolName() const override { return "Gomma"; }
    KeyCode getShortcut() const override { return KeyCode::E; }

    void inspect(IInspector& inspector) override {
        inspector.property("radius", radius)
                 .tag("ui-label", "Area Cancellazione")
                 .tag("ui-type", "slider")
                 .tag("ui-min", 5.0f).tag("ui-max", 150.0f);
    }

    bool onMouseMove(SkPoint worldPos, Engine*, MapEntity&) override {
        lastMousePos = worldPos; 
        return false; 
    }

    bool onMouseDown(SkPoint worldPos, Engine* engine, MapEntity& map) override {
        auto hits = CollisionEngine::hitTestMulti(map.grid, map.world, worldPos, radius);
        
        if (!hits.empty()) {
            engine->getCommands().execute<CommandRemoveElement>(&map, hits);
            return true; 
        }
        return false;
    }

    void drawGhost(SkCanvas* canvas, float zoom) override {
        SkPaint p;
        p.setColor(SkColorSetARGB(80, 255, 50, 50)); 
        p.setStyle(SkPaint::kFill_Style);
        p.setAntiAlias(true);
        canvas->drawCircle(lastMousePos, radius, p);
        
        p.setStyle(SkPaint::kStroke_Style);
        p.setColor(SkColorSetARGB(255, 255, 0, 0));
        p.setStrokeWidth(2.0f / zoom);
        canvas->drawCircle(lastMousePos, radius, p);
    }
};