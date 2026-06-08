// ViewJoinSession.h
#pragma once
#include "../../ui/framework/IAppView.h"
#include <string>

class ViewJoinSession : public IAppView {
public:
    ViewJoinSession() : IAppView("pages/join_session.rml") {}
protected:
    void onEnter() override;
    void onUpdate(float deltaTime) override;
};