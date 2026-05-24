#pragma once
#include "../../ui/framework/IAppView.h"
#include "../../data/entities/TokenEntity.h"
#include <vector>

class ViewSessionDashboard : public IAppView {
public:
    ViewSessionDashboard(int sessionId) 
        : IAppView("pages/session_dashboard.rml"), activeSessionId(sessionId) {}

protected:
    void onEnter() override;
    void onUpdate(float deltaTime) override;

private:
    void drawMenuBar();
    void drawSidePanels();
    void drawMap(); 

    float zoom = 1.0f;
    float panX = 0.0f, panY = 0.0f;
    int gridSize = 50; 
    int activeSessionId;
    std::vector<TokenEntity> activeTokens;
};