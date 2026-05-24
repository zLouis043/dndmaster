#pragma once
#include "../../ui/framework/IAppView.h"

class ViewMainMenu : public IAppView {
public:
    ViewMainMenu() : IAppView("pages/main_menu.rml") {}

protected:
    void onEnter() override;
    void onUpdate(float deltaTime) override;
};