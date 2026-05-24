#include "ViewSessionDashboard.h"
#include "ViewMainMenu.h"
#include "../../core/engine.h"
#include <imgui.h>

void ViewSessionDashboard::onEnter() {
    // 1-to-N: Troviamo tutti i token che appartengono alla mappa 1
    activeTokens = getEngine()->getDB().getByField<TokenEntity>("map_id", 1); 

    // Popoliamo charData per ogni token (Simulazione della JOIN per i dati in memoria)
    for (auto& token : activeTokens) {
        getEngine()->getDB().getById(token.characterId, token.charData);
    }
}

void ViewSessionDashboard::onUpdate(float deltaTime) {
    // ORM N-to-M
    auto sessionNPCs = getEngine()->getDB().getManyToMany<NpcEntity>("SESSION_NPC", "session_id", "npc_id", activeSessionId); 
    
    for (const auto& npc : sessionNPCs) {
        ImGui::Text("%s", npc.name.c_str());
        ImGui::SameLine();
        
        if (ImGui::Button(("Piazza##" + std::to_string(npc.id)).c_str())) {
            int w, h;
            getEngine()->getWindow().getDrawableSize(&w, &h);
            float worldX = ((w / 2.0f) - panX) / zoom;
            float worldY = ((h / 2.0f) - panY) / zoom;
            
            // IL TRIONFO DELL'ORM:
            // Niente SQL per l'inserimento. Riempiamo la struct ed è fatta!
            TokenEntity newToken;
            newToken.mapId = 1;
            newToken.characterId = npc.id;
            newToken.x = worldX;
            newToken.y = worldY;
            getEngine()->getDB().save(newToken);
            
            // Ricarichiamo pulito
            activeTokens = getEngine()->getDB().getByField<TokenEntity>("map_id", 1);
            for (auto& t : activeTokens) getEngine()->getDB().getById(t.characterId, t.charData);
        }
    }
}