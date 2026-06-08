// ViewHostSession.cpp
#include "ViewHostSession.h"
#include "../../core/engine.h"
#include "ViewMainMenu.h"

void ViewHostSession::onEnter() {
    // Mostra spinner di caricamento
    if (auto el = getDocument()->GetElementById("status"))
        el->SetInnerRML("Apertura sessione in corso...");

    // Apre la sessione — il callback arriva quando PocketBase risponde
    getEngine()->getNetwork().hostSession(
        // Player vuole unirsi
        [this](const std::string& id, const std::string& hash) {
            std::cout << "[NET] JoinRequest da: " << id << std::endl;
            return true; // per ora accetta tutti — qui andrà la validazione hash
        },

        // Player disconnesso
        [this](const std::string& id) {
            std::cout << "[NET] Player disconnesso: " << id << std::endl;
        },

        // Messaggio ricevuto
        [this](const NetMessage& msg) {
            std::cout << "[NET] Messaggio: " << (int)msg.type << std::endl;
        },

        // Sessione pronta — mostra il codice
        [this](const std::string& code) {
            defer([this, code]() {
                if (auto el = getDocument()->GetElementById("status"))
                    el->SetInnerRML("Sessione aperta!");
                if (auto el = getDocument()->GetElementById("session_code"))
                    el->SetInnerRML(code);
                if (auto el = getDocument()->GetElementById("code_container"))
                    el->SetProperty("display", "block");
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

    bindEvent("btn_back", Rml::EventId::Click, [this](Rml::Event&) {
        getEngine()->getNetwork().closeSession();
        getEngine()->changeView<ViewMainMenu>();
    });
}

void ViewHostSession::onUpdate(float deltaTime) {}

void ViewHostSession::onExit() {
    // Non chiudiamo la sessione qui — la sessione deve sopravvivere
    // alla navigazione verso ViewSessionDashboard
}