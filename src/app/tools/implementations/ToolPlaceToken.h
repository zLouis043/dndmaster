#pragma once
#include "../IMapTool.h"
#include "../../commands/EditorCommands.h"
#include "../../../data/elements/ElementToken.h"
#include "../../../data/entities/MapEntity.h"

class ToolPlaceToken : public IMapTool {
public:
    float radius = 25.0f;
    SkColor color = SK_ColorMAGENTA;

    std::string getType() const override { return "tool_token"; }
    std::string getToolName() const override { return "Token"; }
    KeyCode getShortcut() const override { return KeyCode::T; }

    void inspect(IInspector& inspector) override {
        inspector.property("radius", radius)
                 .tag("ui-label", "Raggio (px)")
                 .tag("ui-type", "slider")
                 .tag("ui-min", 10.0f).tag("ui-max", 150.0f);
                 
        inspector.property("color", color)
                 .tag("ui-label", "Colore Token")
                 .tag("ui-type", "color");
    }

    bool onMouseDown(SkPoint worldPos, Engine* engine, MapEntity& map) override {
        auto token = std::make_shared<ElementToken>(worldPos);
        token->radius = radius; 
        token->color = color;
        
        engine->getCommands().execute<CommandAddElement>(&map, token);
        return true; 
    }
};