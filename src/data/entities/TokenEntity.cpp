#include "TokenEntity.h"
#include <nlohmann/json.hpp>
#include "../db/DbRegistry.h"
#include "../db/DatabaseEngine.h"
#include "../plugs/JsonPlugs.h"

REGISTER_DB_ENTITY(TokenEntity);

void TokenEntity::inspect(IInspector& inspector) {
    inspector.name("MAP_TOKEN");
    inspector.property("id", id).tag("primary-key", true).tag("ui-ignore", true);
    inspector.property("map_id", mapId).tag("ui-ignore", true);
    inspector.property("character_id", characterId).tag("ui-ignore", true);
    inspector.property("pos_x", x).tag("ui-ignore", true);
    inspector.property("pos_y", y).tag("ui-ignore", true);
    
    inspector.property("charData", charData).tag("db-ignore", true).as<nlohmann::json>();
}