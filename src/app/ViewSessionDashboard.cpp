#include "ViewSessionDashboard.h"
#include "ViewMainMenu.h"
#include "../core/engine.h"
#include <imgui.h>

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>

#include <cmath>

void ViewSessionDashboard::onEnter(Engine* engine) {
    
    // CARICAMENTO TOKEN CON SINTASSI FLUIDA
    std::string sql = QueryBuilder::select({"t.id", "t.map_id", "t.character_id", "t.pos_x", "t.pos_y", "n.name", "n.hp", "n.stats_json"})
        .from("MAP_TOKEN t")
        .join("NPC n ON t.character_id = n.id")
        .where("t.map_id = ?")
        .build();
        
    activeTokens = engine->getDB().getCustom<TokenEntity>(sql, 1); 
}

void ViewSessionDashboard::onUpdate(Engine* engine, float deltaTime) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
    
    float localMouseX = io.MousePos.x - windowPos.x;
    float localMouseY = io.MousePos.y - windowPos.y;
    mouseWorldX = (localMouseX - panX) / zoom;
    mouseWorldY = (localMouseY - panY) / zoom;

    if (!io.WantCaptureMouse) { 
        hoveredTokenId = -1;
        for (auto it = activeTokens.rbegin(); it != activeTokens.rend(); ++it) {
            float dx = mouseWorldX - it->x;
            float dy = mouseWorldY - it->y;
            if (std::sqrt(dx*dx + dy*dy) < 20.0f) { 
                hoveredTokenId = it->id;
                break;
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            selectedTokenId = hoveredTokenId; 
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && selectedTokenId != -1) {
            for (auto& t : activeTokens) {
                if (t.id == selectedTokenId) {
                    t.x = mouseWorldX;
                    t.y = mouseWorldY;
                    break;
                }
            }
        }

        // SALVATAGGIO POSIZIONE SUL DB 
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && selectedTokenId != -1) {
            for (auto& t : activeTokens) {
                if (t.id == selectedTokenId) {
                    std::string updateSql = QueryBuilder::update("MAP_TOKEN").set("pos_x").set("pos_y").where("id = ?").build();
                    engine->getDB().executeCommand(updateSql, t.x, t.y, t.id);
                    break;
                }
            }
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            panX += io.MouseDelta.x;
            panY += io.MouseDelta.y;
        }
        zoom += io.MouseWheel * 0.1f;
        if (zoom < 0.1f) zoom = 0.1f;
    }

    drawMap(engine);
    drawMenuBar(engine);
    drawSidePanels(engine);
}

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>

void ViewSessionDashboard::drawMap(Engine* engine) {
    SkCanvas* canvas = engine->getCanvas();
    if (!canvas) return;

    canvas->save(); // Come nvgSave()
    canvas->translate(panX, panY);
    canvas->scale(zoom, zoom);

    // Esempio: Disegna un token quadrato rosso con Skia
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setAntiAlias(true);

    SkRect rect = SkRect::MakeXYWH(100, 100, 50, 50);
    canvas->drawRect(rect, paint);

    canvas->restore(); // Come nvgRestore()
}

void ViewSessionDashboard::drawMenuBar(Engine* engine) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Salva Campagna")) { /* TODO */ }
            if (ImGui::MenuItem("Torna al Menu")) {
                engine->changeView(std::make_unique<ViewMainMenu>());
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Esci")) { engine->quit(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Visualizza")) {
            ImGui::MenuItem("Pannello NPC", nullptr, nullptr);
            ImGui::MenuItem("Note Lore", nullptr, nullptr);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void ViewSessionDashboard::drawSidePanels(Engine* engine) {
    ImGui::Begin("Roster Sessione");
    
    // CARICARE IL ROSTER CON SINTASSI FLUIDA
    std::string selectSql = QueryBuilder::select({"n.id", "n.name", "n.hp", "n.stats_json"})
                                .from("NPC n")
                                .join("SESSION_NPC sn ON n.id = sn.npc_id")
                                .where("sn.session_id = ?")
                                .build();
    auto sessionNPCs = engine->getDB().getCustom<NpcEntity>(selectSql, activeSessionId); 
    
    for (const auto& npc : sessionNPCs) {
        ImGui::Text("%s", npc.name.c_str());
        ImGui::SameLine();
        
        if (ImGui::Button(("Piazza##" + std::to_string(npc.id)).c_str())) {
            int w, h;
            SDL_GetWindowSize(engine->getWindow(), &w, &h);
            float worldX = ((w / 2.0f) - panX) / zoom;
            float worldY = ((h / 2.0f) - panY) / zoom;
            
            // PIAZZAMENTO TOKEN NEL DB
            std::string insSql = QueryBuilder::insertInto("MAP_TOKEN")
                .column("map_id").column("character_id").column("pos_x").column("pos_y").build();
            engine->getDB().executeCommand(insSql, 1, npc.id, worldX, worldY); 
            
            // RICARICAMENTO
            std::string sql = QueryBuilder::select({"t.id", "t.map_id", "t.character_id", "t.pos_x", "t.pos_y", "n.name", "n.hp", "n.stats_json"})
                .from("MAP_TOKEN t")
                .join("NPC n ON t.character_id = n.id")
                .where("t.map_id = ?")
                .build();
            activeTokens = engine->getDB().getCustom<TokenEntity>(sql, 1);
        }
    }
    ImGui::End();
}

void ViewSessionDashboard::onExit(Engine* engine) {}