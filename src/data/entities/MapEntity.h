#pragma once
#include "../inspector/Inspection.h"
#include "../elements/IMapElement.h" 
#include "../../core/ecs/World.h"
#include "../../core/physics/SpatialGrid.h"
#include <string>
#include <vector>
#include <memory>

class Engine;

class MapEntity : public IInspectable {
public:
    int id = -1;
    int campaignId = 1;
    std::string name = "Nuova Mappa";
    int gridSize = 50;
    
    World<IMapElement> world; 
    SpatialGrid grid;

    EntityId selectedEntityId = 0;

    std::vector<std::shared_ptr<IMapElement>> _serialBuffer;

public:
    MapEntity() : grid(100.0f) {}

    void selectElement(EntityId id, Engine* engine);
    void clearSelection(Engine* engine);

    void rebuildGrid() {
        grid.clear();
        world.forEach([this](EntityId id, IMapElement& el) {
            grid.insert(id, el.getCollider().getBounds());
        });
    }

    void inspect(IInspector& inspector) override;
};