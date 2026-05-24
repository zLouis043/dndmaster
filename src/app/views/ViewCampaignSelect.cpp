#include "ViewCampaignSelect.h"
#include "ViewCampaignHub.h"
#include "../../core/engine.h"
#include "../../data/entities/NpcEntity.h"
#include "../../data/entities/TokenEntity.h"
#include "../../data/entities/SessionEntity.h"
#include "../../data/entities/MapEntity.h"

void ViewCampaignSelect::onEnter() {
    m_formData.createAction = [this]() {
        writeUI(&m_formData, "form_container"); 
        if (!m_formData.name.empty()) {
            setupDatabase(m_formData.name + ".dndcamp");
        }
    };

    m_formData.loadTestAction = [this]() {
        setupDatabase("test_campaign.dndcamp");
    };

    mount("campaign_form_mount", &m_formData, "form_container");
}

void ViewCampaignSelect::onExit() {
    unmount("campaign_form_mount");
}

void ViewCampaignSelect::setupDatabase(const std::string& dbName) {
    if (getEngine()->getDB().open(dbName)) {
        getEngine()->getDB().registerSchema<NpcEntity>();
        getEngine()->getDB().registerSchema<TokenEntity>();
        getEngine()->getDB().registerSchema<SessionEntity>();
        getEngine()->getDB().registerSchema<MapEntity>();
        
        getEngine()->getDB().createJunctionTable("SESSION_NPC", "session_id", "npc_id");
        
        getEngine()->changeView<ViewCampaignHub>();
    }
}