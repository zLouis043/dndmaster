#pragma once
#include "../IMapTool.h"
#include "../../commands/EditorCommands.h"
#include "../../../data/elements/ElementSegment.h"
#include "../../../data/entities/MapEntity.h"
#include <optional>
#include <include/core/SkPaint.h>

class ToolDrawWall : public IMapTool {
public:
    float thickness = 5.0f;
    SkColor color = SK_ColorCYAN;
    
    std::optional<SkPoint> lastDrawPoint;
    SkPoint currentMousePos = SkPoint::Make(0,0);

    std::string getType() const override { return "tool_wall"; }
    std::string getToolName() const override { return "Muro"; }
    KeyCode getShortcut() const override { return KeyCode::W; }

    void inspect(IInspector& inspector) override {
        inspector.property("thickness", thickness)
                 .tag("ui-label", "Spessore")
                 .tag("ui-type", "slider")
                 .tag("ui-min", 1.0f).tag("ui-max", 20.0f);
                 
        inspector.property("color", color)
                 .tag("ui-label", "Colore Muro")
                 .tag("ui-type", "color");
    }

    bool onMouseDown(SkPoint worldPos, Engine* engine, MapEntity& map) override {
        if (lastDrawPoint) {
            auto segment = std::make_shared<ElementSegment>(lastDrawPoint.value(), worldPos);
            segment->thickness = thickness; 
            segment->color = color;
            
            engine->getCommands().execute<CommandAddElement>(&map, segment);
        }
        lastDrawPoint = worldPos;
        return true;
    }

    bool onMouseMove(SkPoint worldPos, Engine*, MapEntity&) override {
        currentMousePos = worldPos; 
        return false;
    }

    void drawGhost(SkCanvas* canvas, float zoom) override {
        if (lastDrawPoint) {
            SkPaint p; 
            p.setColor(color); 
            p.setStrokeWidth(thickness / zoom); 
            p.setAntiAlias(true);
            canvas->drawLine(lastDrawPoint.value(), currentMousePos, p);
        }
    }

    void onCancel() override { 
        lastDrawPoint.reset(); 
    }
};