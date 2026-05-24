#pragma once
#include "../inspector/Inspection.h"
#include <string>

class SessionEntity : public IInspectable {
public:
    int id = -1;
    int campaignId = 1;
    std::string title = "Nuova Sessione";
    int activeMapId = -1; 
    std::string date = "";
    std::string notes = "";
    bool isActive = false;

    void inspect(IInspector& inspector) override;
};