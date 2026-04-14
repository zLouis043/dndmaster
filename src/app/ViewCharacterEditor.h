#pragma once
#include "IAppView.h"
#include "../data/NpcEntity.h" // Sostituisce il vecchio GameModels.h

class ViewCharacterEditor : public IAppView {
public:
    void onEnter(Engine* engine) override;
    void onUpdate(Engine* engine, float deltaTime) override;
    void onExit(Engine* engine) override;

private:
    NpcEntity editingChar;
    char nameBuf[64] = "";
};