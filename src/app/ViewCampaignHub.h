#pragma once
#include "IAppView.h"
#include "../data/NpcEntity.h"
#include "../data/SessionEntity.h"
#include <vector>

class ViewCampaignHub : public IAppView {
public:
    void onEnter(Engine* engine) override;
    void onUpdate(Engine* engine, float deltaTime) override;
    void onExit(Engine* engine) override;

private:
    enum class Tab { Sessions, Characters, Maps };
    Tab currentTab = Tab::Sessions;

    // --- CACHE IN RAM (Niente più query a 60fps!) ---
    std::vector<NpcEntity> allNPCs;
    std::vector<NpcEntity> sessionNPCs;
    std::vector<SessionEntity> allSessions;

    int selectedSessionId = -1;

    // Metodo helper per ricaricare la cache solo quando serve
    void refreshData(Engine* engine);
};