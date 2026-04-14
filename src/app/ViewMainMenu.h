#pragma once
#include "IAppView.h"

class ViewMainMenu : public IAppView {
public:
    void onEnter(Engine* engine) override;
    void onUpdate(Engine* engine, float deltaTime) override;
    void onExit(Engine* engine) override;
};