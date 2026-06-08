// ViewHostSession.cpp
#include "ViewHostSession.h"
#include "../../core/engine.h"
#include "ViewMainMenu.h"

void ViewHostSession::onEnter() {
    if (auto el = getDocument()->GetElementById("status"))
        el->SetInnerRML("Apertura sessione in corso...");

    getEngine()->getNetwork().hostSession(
        [this](const std::string& id, const std::string& hash) {
            std::cout << "[NET] JoinRequest da: " << id << std::endl;
            return true;
        },

        [this](const std::string& id) {
            std::cout << "[NET] Player disconnesso: " << id << std::endl;
        },

        [this](const NetMessage& msg) {
            std::cout << "[NET] Messaggio: " << (int)msg.type << std::endl;

            if (msg.type == MessageType::AssetAvailable) {
                std::string hash = msg.payload.value("hash", "");
                if (!hash.empty()) {
                    getEngine()->getNetwork().fetchAssetAsync(hash);
                }
            }
        },

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

    bindEvent("btn_debug_map", Rml::EventId::Click, [this](Rml::Event&) {
        std::cout << "[SISTEMA] Il Master sta avviando l'upload della mappa in background...\n";
        std::vector<uint8_t> dummyMapData = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
        getEngine()->getNetwork().publishAsset(dummyMapData, ".webp");
    });
}

void ViewHostSession::onUpdate(float deltaTime) {}

void ViewHostSession::onExit() {
}