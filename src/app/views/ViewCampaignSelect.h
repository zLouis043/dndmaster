#pragma once
#include "../../ui/framework/IAppView.h"
#include "../../data/inspector/Inspection.h"
#include <string>

class ViewCampaignSelect : public IAppView {
public:
    ViewCampaignSelect() : IAppView("pages/campaign_select.rml") {}

protected:
    void onEnter() override;
    void onExit() override;

private:
    void setupDatabase(const std::string& dbName);

    // Struttura fittizia che l'AutoInspector userà per generare il Form
    struct CampaignFormData : public IInspectable {
        std::string name = "";
        std::function<void()> createAction;
        std::function<void()> loadTestAction;

        void inspect(IInspector& inspector) override {
            inspector.property("name", name)
                     .tag("ui-label", "Nome (Senza estensione)")
                     .tag("ui-type", "input");
                     
            inspector.property("create", createAction)
                     .tag("ui-label", "Crea Nuova Campagna")
                     .tag("ui-class", "btn-primary");
                     
            inspector.property("load", loadTestAction)
                     .tag("ui-label", "Carica Test Default")
                     .tag("ui-class", "btn-secondary");
        }
    } m_formData;
};