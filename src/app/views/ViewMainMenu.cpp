#include "ViewMainMenu.h"
#include "../../core/engine.h"
#include "ViewCampaignSelect.h"
#include <imgui.h>

void ViewMainMenu::onEnter() {
    bindEvent("btn_master", Rml::EventId::Click, [this](Rml::Event& event) {
        getEngine()->changeView<ViewCampaignSelect>();
    });

    bindEvent("btn_exit", Rml::EventId::Click, [this](Rml::Event& event) {
        getEngine()->quit();
    });
}

void ViewMainMenu::onUpdate(float deltaTime) {
}
