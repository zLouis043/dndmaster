#include "ViewCampaignSelect.h"
#include "ViewCampaignHub.h" // Il prossimo Stato
#include "../core/engine.h"
#include "../data/NpcEntity.h"
#include "../data/TokenEntity.h"
#include "../data/SessionEntity.h" // NUOVO
#include "../data/MapEntity.h"     // NUOVO
#include <imgui.h>

void ViewCampaignSelect::onEnter(Engine *engine)
{
}

void ViewCampaignSelect::onUpdate(Engine *engine, float deltaTime) {
    ImGui::Begin("Gestione Campagne Master", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Crea una Nuova Campagna");
    ImGui::InputText("Nome Campagna", newCampaignName, 64);
    
    // Funzione Lambda locale per non ripetere il codice di setup
    auto setupDatabase = [&](const std::string& dbName) {
        if (engine->getDB().open(dbName)) {
            engine->getDB().setupEntitySchema(NpcEntity{});
            engine->getDB().setupEntitySchema(TokenEntity{});
            engine->getDB().setupEntitySchema(SessionEntity{});
            engine->getDB().setupEntitySchema(MapEntity{});
            
            // Tabella di relazione (Many-to-Many)
            engine->getDB().executeCommand("CREATE TABLE IF NOT EXISTS SESSION_NPC (session_id INTEGER, npc_id INTEGER, PRIMARY KEY(session_id, npc_id))");
            
            engine->changeView(std::make_unique<ViewCampaignHub>());
        }
    };

    if (ImGui::Button("Crea File .dndcamp")) {
        setupDatabase(std::string(newCampaignName) + ".dndcamp");
    }

    ImGui::Separator();
    ImGui::Text("Oppure carica un file esistente");
    
    if (ImGui::Button("Carica Campagna di Test")) {
        setupDatabase("test_campaign.dndcamp");
    }

    ImGui::End();
}

void ViewCampaignSelect::onExit(Engine *engine)
{
}
