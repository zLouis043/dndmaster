#pragma once
#include "../inspector/Inspection.h"
#include <string>

struct CharacterStats : public IInspectable {
    int str = 10, dex = 10, con = 10;
    int intl = 10, wis = 10, cha = 10;
    
    void inspect(IInspector& inspector) override;
};

class NpcEntity : public IInspectable {
public:
    int id = -1;
    std::string name = "Nuovo Eroe";
    int hp = 10;
    CharacterStats stats;

    void inspect(IInspector& inspector) override;
};