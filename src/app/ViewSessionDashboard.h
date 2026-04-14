#pragma once
#include "IAppView.h"
#include "../data/TokenEntity.h" // Addio GameModels.h!
#include <vector>

class ViewSessionDashboard : public IAppView {
public:
    ViewSessionDashboard(int sessionId) : activeSessionId(sessionId) {}

    void onEnter(Engine* engine) override;
    void onUpdate(Engine* engine, float deltaTime) override;
    void onExit(Engine* engine) override;

private:
    void drawMenuBar(Engine* engine);
    void drawSidePanels(Engine* engine);
    void drawMap(Engine* engine); 

    float zoom = 1.0f;
    float panX = 0.0f;
    float panY = 0.0f;
    int gridSize = 50; 
    int activeSessionId;
    float mouseWorldX = 0.0f;
    float mouseWorldY = 0.0f;
    
    // ORA USIAMO LE NUOVE ENTITÀ!
    std::vector<TokenEntity> activeTokens;
    int selectedTokenId = -1;
    int hoveredTokenId = -1;
};