#include "MapEntity.h"
#include "../db/DbRegistry.h"
#include "../../core/engine.h"
#include "../../app/events/EditorEvents.h"
#include "../plugs/JsonPlugs.h" 
#include "../plugs/WorldPlug.h" 

#include "../elements/MapElementFactory.h"

REGISTER_DB_ENTITY(MapEntity)

void MapEntity::selectElement(EntityId id, Engine* engine) {
    if (selectedEntityId != id) {
        selectedEntityId = id;
        if (engine) engine->getEvents().dispatch(EventSelectionChanged{});
    }
}

void MapEntity::clearSelection(Engine* engine) {
    selectElement(0, engine);
}

void MapEntity::inspect(IInspector& inspector) {
    inspector.name("MAP");
    
    inspector.property("id", id).tag("primary-key", true).tag("ui-ignore", true);
    inspector.property("campaign_id", campaignId).tag("ui-ignore", true);
    inspector.property("name", name).tag("ui-label", "Nome Mappa").tag("ui-type", "input");
    inspector.property("grid_size", gridSize).tag("ui-label", "Griglia (px)").tag("ui-type", "slider");
    inspector.property("elements", world)
             .as<nlohmann::json>()
             .tag("ui-ignore", true);
}