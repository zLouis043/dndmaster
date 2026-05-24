#include "ViewCharacterEditor.h"
#include "ViewCampaignHub.h"
#include "../../core/engine.h"

void ViewCharacterEditor::onEnter() {
    // 1. Caricamento pulito tramite il nuovo getById
    if (targetId != -1) {
        getEngine()->getDB().getById<NpcEntity>(targetId, editingChar);
    }

    // 2. Montiamo i dati nel DOM: verranno generati gli input e riempiti in automatico!
    mount("char_editor_mount", &editingChar, "form_container");

    // 3. Binding dei bottoni
    bindEvent("btn_cancel", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->changeView<ViewCampaignHub>();
    });

    bindEvent("btn_save", Rml::EventId::Click, [this](Rml::Event&) {
        // LEGGIAMO DALLA UI! writeUI prende ciò che c'è scritto nei box HTML e lo butta in editingChar
        writeUI(&editingChar, "form_container"); 
        
        getEngine()->getDB().save(editingChar);
        getEngine()->changeView<ViewCampaignHub>();
    });
}

void ViewCharacterEditor::onExit() {
    unmount("char_editor_mount");
}