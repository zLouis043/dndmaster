#include "ViewMainMenu.h"
#include "../../core/engine.h"
#include "ViewCampaignSelect.h" // Per il cambio scena
#include <imgui.h>

void ViewMainMenu::onEnter() {
    bindEvent("btn_master", Rml::EventId::Click, [this](Rml::Event& event) {
        getEngine()->changeView<ViewCampaignSelect>();
    });

    // 2. Bottone "Carica Test"
    bindEvent("btn_exit", Rml::EventId::Click, [this](Rml::Event& event) {
        getEngine()->quit();
    });
}

void ViewMainMenu::onUpdate(float deltaTime) {
}
