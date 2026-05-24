#pragma once
#include "../../ui/framework/IAppView.h"
#include "../../data/entities/NpcEntity.h"

class ViewCharacterEditor : public IAppView {
public:
    ViewCharacterEditor(int npcId = -1) : IAppView("pages/character_editor.rml"), targetId(npcId) {}

protected:
    void onEnter() override;
    void onExit() override;

private:
    int targetId;
    NpcEntity editingChar;
};