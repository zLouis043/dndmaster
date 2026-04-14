#pragma once
#include "IAppView.h"
#include <string>
#include <vector>

class ViewCampaignSelect : public IAppView {
public:
    void onEnter(Engine* engine) override;
    void onUpdate(Engine* engine, float deltaTime) override;
    void onExit(Engine* engine) override;

private:
    char newCampaignName[64] = "";
};