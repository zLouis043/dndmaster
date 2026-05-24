#include "MapEntity.h"
#include "../db/DbRegistry.h"
#include "../db/DatabaseEngine.h"
#include "../../core/engine.h"
#include "../../app/events/EditorEvents.h"
#include "../plugs/JsonPlugs.h" 
#include "../elements/MapElementFactory.h"

REGISTER_DB_ENTITY(MapEntity)

void MapEntity::selectElement(std::shared_ptr<IMapElement> el, Engine* engine) {
    if (selectedElement != el) {
        selectedElement = el;
        if (engine) {
            engine->getEvents().dispatch(EventSelectionChanged{});
        }
    }
}

void MapEntity::clearSelection(Engine* engine) {
    selectElement(nullptr, engine);
}

void MapEntity::inspect(IInspector& inspector) {
    inspector.name("MAP");
    
    inspector.property("id", id).tag("primary-key", true).tag("ui-ignore", true);
    inspector.property("campaign_id", campaignId).tag("ui-ignore", true);
    
    inspector.property("name", name)
             .tag("ui-label", "Nome Mappa")
             .tag("ui-type", "input");
             
    inspector.property("grid_size", gridSize)
             .tag("ui-label", "Griglia (px)")
             .tag("ui-type", "slider")
             .tag("ui-min", 10.0f).tag("ui-max", 150.0f);
             
    inspector.property("elements", elements)
             .tag("ui-ignore", true)
             .as<nlohmann::json>();
}