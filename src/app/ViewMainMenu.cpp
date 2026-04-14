#include "ViewMainMenu.h"
#include "../core/engine.h"
#include "ViewCampaignSelect.h"
#include <imgui.h>
#include <iostream>

void ViewMainMenu::onEnter(Engine* engine) {
    // Setup iniziale (ad esempio caricare sfondi o file di configurazione)
}

void ViewMainMenu::onUpdate(Engine* engine, float deltaTime) {
    // Rendiamo la finestra di ImGui a tutto schermo e fissa
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
                                    ImGuiWindowFlags_NoMove | 
                                    ImGuiWindowFlags_NoSavedSettings | 
                                    ImGuiWindowFlags_NoBringToFrontOnFocus;
    
    ImGui::Begin("Main Menu", nullptr, window_flags);
    
    // Centriamo i bottoni calcolando la dimensione dello schermo
    ImVec2 windowSize = ImGui::GetWindowSize();
    float buttonWidth = 300.0f;
    float buttonHeight = 50.0f;
    
    ImGui::SetCursorPos(ImVec2((windowSize.x - buttonWidth) * 0.5f, windowSize.y * 0.3f));
    
    ImGui::Text("=== DnDMaster ===");
    ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spazio vuoto
    
    // Bottoni (per ora vuoti, aggiungeremo gli Viewi prossimamente)
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);
    if (ImGui::Button("Workspace Master (Offline)", ImVec2(buttonWidth, buttonHeight))) {
        engine->changeView(std::make_unique<ViewCampaignSelect>());
    }
    
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);
    if (ImGui::Button("Workspace Player (Offline)", ImVec2(buttonWidth, buttonHeight))) {
        // TODO: engine->changeView(std::make_unique<ViewPlayerWorkspace>());
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);
    if (ImGui::Button("Unisciti a Sessione Live", ImVec2(buttonWidth, buttonHeight))) {
        // TODO: engine->changeView(std::make_unique<ViewLiveSession>());
    }

    ImGui::Dummy(ImVec2(0.0f, 30.0f));
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);
    if (ImGui::Button("Esci Dal Gioco", ImVec2(buttonWidth, buttonHeight))) {
        engine->quit(); // Comunichiamo all'engine di chiudere!
    }

    ImGui::End();
}

void ViewMainMenu::onExit(Engine* engine) {
    // Cleanup (liberare texture, ecc.)
}