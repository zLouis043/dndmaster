#include "ViewCharacterEditor.h"
#include "ViewCampaignHub.h"
#include "../../core/engine.h"

void ViewCharacterEditor::onEnter() {
    if (targetId != -1) {
        getEngine()->getDB().getById<NpcEntity>(targetId, editingChar);
    }

    mount("char_editor_mount", &editingChar, "form_container");

    bindEvent("btn_cancel", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->changeView<ViewCampaignHub>();
    });

    bindEvent("btn_save", Rml::EventId::Click, [this](Rml::Event&) {
        writeUI(&editingChar, "form_container"); 
        
        getEngine()->getDB().save(editingChar);
        getEngine()->changeView<ViewCampaignHub>();
    });
}

void ViewCharacterEditor::onExit() {
    unmount("char_editor_mount");
}