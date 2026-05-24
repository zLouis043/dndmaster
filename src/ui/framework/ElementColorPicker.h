#pragma once
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementInstancer.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Dictionary.h>
#include <RmlUi/Core/EventListener.h>
#include "RmlWidget.h"
#include <string>
#include <format>
#include <iostream>

class ElementColorPicker : public Rml::Element, public Rml::EventListener {
private:
    bool m_isOpen = false;
    Rml::Element* m_popup = nullptr;
    int m_r = 255, m_g = 255, m_b = 255;

public:
    ElementColorPicker(const Rml::String& tag) : Rml::Element(tag) {
        SetProperty("display", "inline-block");
        SetProperty("width", "40px");
        SetProperty("height", "25px");
        SetProperty("border", "2px solid #333");
        SetProperty("cursor", "pointer");
        SetProperty("position", "relative"); 
        
        AddEventListener(Rml::EventId::Change, this, true);
    }

    virtual ~ElementColorPicker() {
        RemoveEventListener(Rml::EventId::Change, this, true);
    }

    void ProcessEvent(Rml::Event& event) override {
        if (event.GetId() == Rml::EventId::Change) {
            auto target = event.GetTargetElement();
            
            if (target != this && m_popup) {
                event.StopPropagation();

                float rawVal = event.GetParameter<float>("value", -1.0f);
                if (rawVal < 0.0f) return; 

                int v = static_cast<int>(rawVal);

                if (target->GetId() == "cp_r") m_r = v;
                else if (target->GetId() == "cp_g") m_g = v;
                else if (target->GetId() == "cp_b") m_b = v;

                std::string newHex = std::format("#{:02X}{:02X}{:02X}", m_r, m_g, m_b);
                
                SetAttribute("value", newHex);

                Rml::Dictionary params;
                params["value"] = newHex;
                DispatchEvent(Rml::EventId::Change, params);
            }
        }
    }

protected:
    void OnAttributeChange(const Rml::Dictionary& changed_attributes) override {
        Rml::Element::OnAttributeChange(changed_attributes);
        if (changed_attributes.find("value") != changed_attributes.end()) {
            Rml::String hex = GetAttribute<Rml::String>("value", "#FFFFFF");

            if (hex.length() >= 7 && hex[0] == '#') {
                try {
                    int r = std::stoi(hex.substr(1, 2), nullptr, 16);
                    int g = std::stoi(hex.substr(3, 2), nullptr, 16);
                    int b = std::stoi(hex.substr(5, 2), nullptr, 16);
                    
                    std::string safeCss = std::format("rgb({}, {}, {})", r, g, b);
                    SetProperty("background-color", safeCss);
                } catch(...) {
                    SetProperty("background-color", "rgb(255, 255, 255)");
                }
            }
        }
    }

    void ProcessDefaultAction(Rml::Event& event) override {
        Rml::Element::ProcessDefaultAction(event);
        if (event.GetId() == Rml::EventId::Click && event.GetTargetElement() == this) {
            if (m_isOpen) ClosePopup();
            else OpenPopup();
        }
    }

private:
    void OpenPopup() {
        if (m_isOpen) return;

        std::string rml = RmlWidget::LoadRmlFromFile("assets/ui/widgets/color_picker_popup.rml");
        
        auto popupPtr = GetOwnerDocument()->CreateElement("div");
        m_popup = popupPtr.get();
        m_popup->SetInnerRML(rml);
        
        Rml::String val = GetAttribute<Rml::String>("value", "#FFFFFF");
        if (val.length() >= 7 && val[0] == '#') {
            try {
                m_r = std::stoi(val.substr(1, 2), nullptr, 16);
                m_g = std::stoi(val.substr(3, 2), nullptr, 16);
                m_b = std::stoi(val.substr(5, 2), nullptr, 16);
            } catch(...) {}
        }

        AppendChild(std::move(popupPtr));
        
        auto doc = GetOwnerDocument();
        if (auto er = doc->GetElementById("cp_r")) er->SetAttribute("value", std::to_string(m_r));
        if (auto eg = doc->GetElementById("cp_g")) eg->SetAttribute("value", std::to_string(m_g));
        if (auto eb = doc->GetElementById("cp_b")) eb->SetAttribute("value", std::to_string(m_b));

        m_isOpen = true;
    }

    void ClosePopup() {
        if (!m_isOpen) return;
        RemoveChild(m_popup);
        m_popup = nullptr;
        m_isOpen = false;
    }
};

class ColorPickerInstancer : public Rml::ElementInstancer {
public:
    Rml::ElementPtr InstanceElement(Rml::Element* parent, const Rml::String& tag, const Rml::XMLAttributes& attributes) override {
        return Rml::ElementPtr(new ElementColorPicker(tag));
    }
    void ReleaseElement(Rml::Element* element) override { delete element; }
};