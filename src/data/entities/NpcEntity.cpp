#include "NpcEntity.h"
#include <nlohmann/json.hpp>
#include "../db/DbRegistry.h"
#include "../db/DatabaseEngine.h"
#include "../plugs/JsonPlugs.h" 

REGISTER_DB_ENTITY(NpcEntity);

void CharacterStats::inspect(IInspector& inspector) {
    inspector.property("str", str).tag("ui-label", "FOR (Forza)");
    inspector.property("dex", dex).tag("ui-label", "DES (Destrezza)");
    inspector.property("con", con).tag("ui-label", "COS (Costituzione)");
    inspector.property("intl", intl).tag("ui-label", "INT (Intelligenza)");
    inspector.property("wis", wis).tag("ui-label", "SAG (Saggezza)");
    inspector.property("cha", cha).tag("ui-label", "CAR (Carisma)");
}

void NpcEntity::inspect(IInspector& inspector) {
    inspector.name("NPC");
    inspector.property("id", id).tag("primary-key", true).tag("ui-ignore", true);
    
    inspector.property("name", name).tag("ui-label", "Nome NPC").tag("ui-type", "input");
    inspector.property("hp", hp).tag("ui-label", "Punti Ferita (HP)");
    
    inspector.property("stats", stats)
             .tag("db-ignore", true)
             .tag("ui-label", "Caratteristiche Base");
             
    inspector.property("stats_json", stats)
             .as<nlohmann::json>()
             .tag("ui-ignore", true);
}