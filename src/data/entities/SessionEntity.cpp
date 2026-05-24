#include "SessionEntity.h"
#include "../db/DbRegistry.h"
#include "../db/DatabaseEngine.h"

REGISTER_DB_ENTITY(SessionEntity);

void SessionEntity::inspect(IInspector& inspector) {
    inspector.name("SESSION");

    inspector.property("id", id).tag("primary-key", true).tag("ui-ignore", true);
    inspector.property("campaign_id", campaignId).tag("ui-ignore", true);
    
    inspector.property("title", title).tag("ui-label", "Titolo Sessione").tag("ui-type", "input");
    inspector.property("active_map_id", activeMapId).tag("ui-ignore", true);
    inspector.property("date", date).tag("ui-label", "Data").tag("ui-type", "input");
    inspector.property("notes", notes).tag("ui-label", "Note").tag("ui-type", "input");
    inspector.property("is_active", isActive).tag("ui-ignore", true);
}