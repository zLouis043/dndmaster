#pragma once
#include "../../ui/framework/IAppView.h"
#include "../../data/inspector/DyInspectable.h"
#include "../../data/entities/NpcEntity.h"
#include "../../data/entities/SessionEntity.h"
#include "../../data/entities/MapEntity.h"
#include <vector>

class ViewCampaignHub : public IAppView {
public:
    ViewCampaignHub() : IAppView("pages/campaign_hub.rml") {}

protected:
    void onEnter() override;
    void onExit() override;

private:
    std::vector<SessionEntity> allSessions;
    std::vector<NpcEntity> allNPCs;
    std::vector<MapEntity> allMaps;
    
    std::vector<NpcEntity> sessionNPCs;
    std::vector<NpcEntity> rosterOutNPCs; // Aggiunto per tracciare chi sta fuori!

    DyInspectable<SessionEntity> m_dySessions;
    DyInspectable<NpcEntity> m_dyCharacters;
    DyInspectable<MapEntity> m_dyMaps;
    DyInspectable<NpcEntity> m_dyRosterIn;
    DyInspectable<NpcEntity> m_dyRosterOut;

    int selectedSessionId = -1;

    void refreshData();
    void selectSession(int id);
    void toggleRoster(int npcId, bool addToSession);
};