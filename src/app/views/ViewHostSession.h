// ViewHostSession.h
#pragma once
#include "../../ui/framework/IAppView.h"

class ViewHostSession : public IAppView {
public:
    ViewHostSession() : IAppView("pages/host_session.rml") {}
protected:
    void onEnter() override;
    void onUpdate(float deltaTime) override;
    void onExit() override;
};