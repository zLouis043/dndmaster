#include "ViewMainMenu.h"
#include "../../core/engine.h"
#include "ViewCampaignSelect.h"
#include "ViewJoinSession.h"
#include "ViewHostSession.h"
#include <imgui.h>

void ViewMainMenu::onEnter() {
    bindEvent("btn_master", Rml::EventId::Click, [this](Rml::Event& event) {
        getEngine()->changeView<ViewCampaignSelect>();
    });

    bindEvent("btn_host", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->changeView<ViewHostSession>();
    });

    bindEvent("btn_live", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->changeView<ViewJoinSession>();
    });

    bindEvent("btn_exit", Rml::EventId::Click, [this](Rml::Event& event) {
        getEngine()->quit();
    });
}

void ViewMainMenu::onUpdate(float deltaTime) {
}
