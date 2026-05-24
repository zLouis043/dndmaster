#pragma once
#include "../inspector/Inspection.h"
#include "../elements/IMapElement.h" 
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
    
    std::vector<std::shared_ptr<IMapElement>> elements; 
    std::shared_ptr<IMapElement> selectedElement = nullptr;
    
private:
    SpatialGrid m_spatialGrid;
    bool m_isGridDirty = true;

public:
    void selectElement(std::shared_ptr<IMapElement> el, Engine* engine);
    void clearSelection(Engine* engine);

    void markGridDirty() { m_isGridDirty = true; }
    
    const SpatialGrid& getGrid() {
        if (m_isGridDirty) {
            m_spatialGrid.build(elements);
            m_isGridDirty = false;
        }
        return m_spatialGrid;
    }

    void inspect(IInspector& inspector) override;
};