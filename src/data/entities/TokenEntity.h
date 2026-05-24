#pragma once
#include "../inspector/Inspection.h"
#include "NpcEntity.h"

class TokenEntity : public IInspectable {
public:
    int id = -1;
    int mapId = 1;
    int characterId = -1;
    float x = 0.0f;
    float y = 0.0f;
    
    NpcEntity charData;

    void inspect(IInspector& inspector) override;
};