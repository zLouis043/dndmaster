// ViewJoinSession.cpp
#include "ViewJoinSession.h"
#include "../../core/engine.h"
#include "ViewMainMenu.h"

void ViewJoinSession::onEnter() {
    bindEvent("btn_join", Rml::EventId::Click, [this](Rml::Event&) {
        auto inputEl = getDocument()->GetElementById("input_code");
        if (!inputEl) return;

        std::string code = inputEl->GetAttribute("value", std::string(""));
        if (code.empty()) return;

        if (auto el = getDocument()->GetElementById("status"))
            el->SetInnerRML("Connessione in corso...");

        getEngine()->getNetwork().joinSession(
            code,
            "Giocatore",    // qui andrà il nome reale dal personaggio
            "hash-123",     // qui andrà l'hash reale del personaggio

            // Messaggio ricevuto
            [this](const NetMessage& msg) {
                std::cout << "[NET] Messaggio: " << (int)msg.type << std::endl;
            },

            // Connesso
            [this](const std::string& code) {
                defer([this]() {
                    if (auto el = getDocument()->GetElementById("status"))
                        el->SetInnerRML("Connesso!");
                    // getEngine()->changeView<ViewSessionPlayer>();
                });
            },

            // Errore
            [this](const std::string& err) {
                defer([this, err]() {
                    if (auto el = getDocument()->GetElementById("status"))
                        el->SetInnerRML("Errore: " + err);
                });
            }
        );
    });

    bindEvent("btn_back", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->changeView<ViewMainMenu>();
    });
}

void ViewJoinSession::onUpdate(float deltaTime) {}