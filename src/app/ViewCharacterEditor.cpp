#include "ViewCharacterEditor.h"
#include "ViewCampaignHub.h"
#include "../core/engine.h"
#include <imgui.h>
#include <cstring>

void ViewCharacterEditor::onEnter(Engine* engine) {
    // Reset o inizializzazione buffer
    std::strncpy(nameBuf, editingChar.name.c_str(), sizeof(nameBuf));
}

void ViewCharacterEditor::onUpdate(Engine* engine, float deltaTime) {
    ImGui::Begin("Editor Personaggio / NPC", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Anagrafica");
    ImGui::InputText("Nome", nameBuf, IM_ARRAYSIZE(nameBuf));
    ImGui::SliderInt("Punti Ferita (HP)", &editingChar.hp, 1, 500);

    ImGui::Separator();
    ImGui::Text("Caratteristiche Base");
    
    // Corretto l'uso di 'stats' al posto del vecchio 'Views' sfalsato!
    ImGui::DragInt("FOR (Str)", &editingChar.stats.str, 0.1f, 1, 30);
    ImGui::DragInt("DES (Dex)", &editingChar.stats.dex, 0.1f, 1, 30);
    ImGui::DragInt("COS (Con)", &editingChar.stats.con, 0.1f, 1, 30);
    ImGui::DragInt("INT (Int)", &editingChar.stats.intl, 0.1f, 1, 30);
    ImGui::DragInt("SAG (Wis)", &editingChar.stats.wis, 0.1f, 1, 30);
    ImGui::DragInt("CAR (Cha)", &editingChar.stats.cha, 0.1f, 1, 30);

    ImGui::Dummy(ImVec2(0, 20));
    
    if (ImGui::Button("Salva nel Database", ImVec2(150, 40))) {
        editingChar.name = nameBuf;
        // Il motore cieco riconosce che NpcEntity eredita da IDatabaseEntity!
        engine->getDB().saveEntity(editingChar); 
        engine->changeView(std::make_unique<ViewCampaignHub>());
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Annulla", ImVec2(150, 40))) {
        engine->changeView(std::make_unique<ViewCampaignHub>());
    }

    ImGui::End();
}

void ViewCharacterEditor::onExit(Engine* engine) {}