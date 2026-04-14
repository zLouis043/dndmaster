#include "ViewCampaignHub.h"
#include "ViewSessionDashboard.h"
#include "ViewCharacterEditor.h"
#include "../core/engine.h"
#include "ViewMainMenu.h"
#include <imgui.h>

void ViewCampaignHub::onEnter(Engine *engine) {
    // Carichiamo i dati dal DB alla RAM una sola volta all'avvio
    refreshData(engine);
}

void ViewCampaignHub::refreshData(Engine* engine) {
    // 1. Ricarica tutte le sessioni
    allSessions = engine->getDB().getAll<SessionEntity>();
    if (!allSessions.empty() && selectedSessionId == -1) {
        selectedSessionId = allSessions[0].id; // Seleziona la prima di default
    }

    // 2. Ricarica tutti gli NPC
    allNPCs = engine->getDB().getAll<NpcEntity>();

    // 3. Ricarica gli NPC della sessione attualmente selezionata
    sessionNPCs.clear();
    if (selectedSessionId != -1) {
        std::string selectSql = QueryBuilder::select({"n.id", "n.name", "n.hp", "n.stats_json"})
                                    .from("NPC n")
                                    .join("SESSION_NPC sn ON n.id = sn.npc_id")
                                    .where("sn.session_id = ?")
                                    .build();
        sessionNPCs = engine->getDB().getCustom<NpcEntity>(selectSql, selectedSessionId);
    }
}

void ViewCampaignHub::onUpdate(Engine *engine, float deltaTime) {
    ImGui::Begin("Hub Campagna", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Campagna")) {
            if (ImGui::MenuItem("Chiudi Campagna")) engine->changeView(std::make_unique<ViewMainMenu>());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImGui::Button("SESSIONI")) currentTab = Tab::Sessions; ImGui::SameLine();
    if (ImGui::Button("PERSONAGGI / NPC")) currentTab = Tab::Characters; ImGui::SameLine();
    if (ImGui::Button("MAPPE")) currentTab = Tab::Maps;

    ImGui::Separator();

    switch (currentTab) {
        case Tab::Sessions: {
            // --- LAYOUT A DUE COLONNE ---
            ImGui::BeginChild("SessionList", ImVec2(200, 0), true);
            ImGui::Text("Elenco Sessioni");
            
            // Bottone per creare una nuova sessione
            if (ImGui::Button("+ Nuova Sessione", ImVec2(-FLT_MIN, 0))) {
                SessionEntity newSession;
                newSession.title = "Sessione " + std::to_string(allSessions.size() + 1);
                engine->getDB().saveEntity(newSession);
                refreshData(engine); // Aggiorna la cache!
            }
            ImGui::Separator();
            
            // Variabile temporanea per ricordare il click
            int clickedSessionId = -1; 

            // Lista cliccabile delle sessioni
            for (const auto& session : allSessions) {
                bool isSelected = (session.id == selectedSessionId);
                if (ImGui::Selectable(session.title.c_str(), isSelected)) {
                    clickedSessionId = session.id; // Segnamo l'ID senza distruggere il vettore!
                }
            }

            // Se abbiamo cliccato qualcosa, facciamo il refresh ORA, in modo sicuro
            if (clickedSessionId != -1) {
                selectedSessionId = clickedSessionId;
                refreshData(engine);
            }
            
            ImGui::EndChild();
            ImGui::SameLine();
            // --- DETTAGLI DELLA SESSIONE SELEZIONATA ---
            ImGui::BeginChild("SessionDetail", ImVec2(0, 0), false);
            if (selectedSessionId != -1) {
                ImGui::Text("Sessione Selezionata ID: %d", selectedSessionId);
                
                if (ImGui::Button("ENTRA IN SESSIONE", ImVec2(200, 40))) {
                    engine->changeView(std::make_unique<ViewSessionDashboard>(selectedSessionId));
                }

                ImGui::Separator();
                ImGui::Text("Gestisci Roster Sessione:");
                
                ImGui::BeginChild("InSession", ImVec2(250, 300), true);
                ImGui::TextDisabled("Presenti (Clicca per Rimuovere):");
                for(auto& npc : sessionNPCs) {
                    if (ImGui::Selectable(npc.name.c_str())) {
                        std::string delSql = QueryBuilder::deleteFrom("SESSION_NPC").where("session_id = ? AND npc_id = ?").build();
                        engine->getDB().executeCommand(delSql, selectedSessionId, npc.id);
                        refreshData(engine);
                    }
                }
                ImGui::EndChild();
                
                ImGui::SameLine();
                
                ImGui::BeginChild("Available", ImVec2(250, 300), true);
                ImGui::TextDisabled("Archivio (Clicca per Aggiungere):");
                for(auto& npc : allNPCs) {
                    if (ImGui::Selectable(npc.name.c_str())) {
                        std::string insSql = QueryBuilder::insertInto("SESSION_NPC").orIgnore().column("session_id").column("npc_id").build();
                        engine->getDB().executeCommand(insSql, selectedSessionId, npc.id);
                        refreshData(engine);
                    }
                }
                ImGui::EndChild();
            } else {
                ImGui::TextDisabled("Nessuna sessione selezionata. Creane una nuova.");
            }
            ImGui::EndChild();
        } break;

        case Tab::Characters: {
            if (ImGui::Button("+ Crea Nuovo NPC")) {
                engine->changeView(std::make_unique<ViewCharacterEditor>());
            }
            ImGui::Separator();
            // Legge dalla RAM, non da SQLite!
            for (auto& npc : allNPCs) {
                if (ImGui::Selectable(npc.name.c_str())) {
                    // TODO: Aprire l'editor
                }
            }
        } break;

        case Tab::Maps:
            ImGui::Text("Gestione Mappe (Lavori in corso...)");
            break;
    }

    ImGui::End();
}

void ViewCampaignHub::onExit(Engine *engine) {}