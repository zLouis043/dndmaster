#pragma once
#include "../../data/inspector/Inspection.h" 
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Element.h>
#include <vector>
#include <string>

class RmlUiAutoInspector : public IInspector {
public:
    enum class Mode { GENERATE, READ, WRITE };

private:
    Rml::ElementDocument* m_doc;
    Mode m_mode;
    
    std::vector<Rml::Element*> m_domStack; 
    std::vector<std::string> m_pathStack;

    std::string getCurrentPath(const std::string& name) const {
        std::string path = "ui";
        for (const auto& p : m_pathStack) path += "_" + p;
        return path + "_" + name;
    }

    Rml::Element* resolveTargetContainer(const PropertyTags& tags) {
        std::string targetId = tags.get<std::string>("ui-target", "");
        if (!targetId.empty()) {
            if (auto el = m_doc->GetElementById(targetId)) return el;
        }
        return m_domStack.empty() ? nullptr : m_domStack.back();
    }

public:
    RmlUiAutoInspector(Rml::ElementDocument* doc, Rml::Element* container, Mode mode) 
        : m_doc(doc), m_mode(mode) {
        if (container) m_domStack.push_back(container);
    }

    ~RmlUiAutoInspector() override = default;

    void name(const std::string&) override {}
    bool isReading() const override { return m_mode == Mode::READ; }

    void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) override {
        if (tags.get<bool>("ui-ignore", false)) return;

        std::string uiId = tags.get<std::string>("ui-id", getCurrentPath(name));
        std::string uiType = tags.get<std::string>("ui-type", "input");
        std::string label = tags.get<std::string>("ui-label", name);

        if (m_mode == Mode::GENERATE) {
            Rml::Element* targetContainer = resolveTargetContainer(tags);
            if (!targetContainer) return;

            Rml::ElementPtr rowNode = m_doc->CreateElement("div");
            rowNode->SetClassNames("prop-row " + tags.get<std::string>("ui-row-class", ""));

            Rml::ElementPtr labelNode = m_doc->CreateElement("label");
            labelNode->SetAttribute("style", "margin-right: 10px;");
            labelNode->SetInnerRML(label);
            rowNode->AppendChild(std::move(labelNode));

            std::string htmlTag = "input";
            if (uiType == "text") htmlTag = "span";
            else if (uiType == "color") htmlTag = "color-picker";

            Rml::ElementPtr inputNode = m_doc->CreateElement(htmlTag);
            inputNode->SetId(uiId);
            if (tags.get<std::string>("ui-class", "") != "") inputNode->SetClassNames(tags.get<std::string>("ui-class", ""));
            
            if (uiType == "slider") {
                inputNode->SetAttribute("type", "range");
                inputNode->SetAttribute("min", std::to_string(tags.get<float>("ui-min", 0.0f)));
                inputNode->SetAttribute("max", std::to_string(tags.get<float>("ui-max", 100.0f)));
                if (t == Type::FLOAT) inputNode->SetAttribute("step", "0.1");
                else inputNode->SetAttribute("step", "1");
            } else if (uiType == "checkbox") {
                inputNode->SetAttribute("type", "checkbox");
            } else if (uiType == "input") {
                inputNode->SetAttribute("type", (t == Type::STRING) ? "text" : "number");
            }

            if (tags.get<bool>("ui-readonly", false)) inputNode->SetAttribute("disabled", "disabled");

            rowNode->AppendChild(std::move(inputNode));
            
            targetContainer->AppendChild(std::move(rowNode));
            return;
        }

        auto element = m_doc->GetElementById(uiId);
        if (!element || tags.get<bool>("ui-readonly", false)) return;

        if (m_mode == Mode::READ && uiType != "text") {
            std::string valStr;
            if (uiType == "checkbox") valStr = element->HasAttribute("checked") ? "1" : "0";
            else valStr = element->GetAttribute("value")->Get<Rml::String>();
            if (valStr.empty()) return;
            try {
                if (uiType == "color" && t == Type::UINT32) {
                    if (valStr.length() >= 7 && valStr[0] == '#') {
                        *static_cast<uint32_t*>(valuePtr) = std::stoul(valStr.substr(1), nullptr, 16) | 0xFF000000;
                    }
                } else {
                    switch (t) {
                        case Type::INT:    *static_cast<int*>(valuePtr) = std::stoi(valStr); break;
                        case Type::FLOAT:  *static_cast<float*>(valuePtr) = std::stof(valStr); break;
                        case Type::STRING: *static_cast<std::string*>(valuePtr) = valStr; break;
                        case Type::BOOL:   *static_cast<bool*>(valuePtr) = (valStr == "true" || valStr == "1"); break;
                        case Type::UINT32: *static_cast<uint32_t*>(valuePtr) = std::stoul(valStr); break;
                    }
                }
            } catch (...) {}
        } else if (m_mode == Mode::WRITE) {
            std::string valStr;
            if (uiType == "color" && t == Type::UINT32) {
                char hex[16];
                snprintf(hex, sizeof(hex), "#%06X", (*static_cast<uint32_t*>(valuePtr)) & 0xFFFFFF);
                valStr = hex;
            } else {
                switch (t) {
                    case Type::INT:    valStr = std::to_string(*static_cast<int*>(valuePtr)); break;
                    case Type::FLOAT:  valStr = std::to_string(*static_cast<float*>(valuePtr)); valStr = valStr.substr(0, valStr.find('.') + 3); break;
                    case Type::STRING: valStr = *static_cast<std::string*>(valuePtr); break;
                    case Type::BOOL:   valStr = *static_cast<bool*>(valuePtr) ? "1" : "0"; break;
                    case Type::UINT32: valStr = std::to_string(*static_cast<uint32_t*>(valuePtr)); break;
                }
            }
            if (uiType == "text") element->SetInnerRML(valStr);
            else if (uiType == "checkbox") {
                if (*static_cast<bool*>(valuePtr)) element->SetAttribute("checked", "checked");
                else element->RemoveAttribute("checked");
            } else element->SetAttribute("value", valStr);
        }
    }

    void onInspectable(const std::string& name, IInspectable& value, const PropertyTags& tags) override {
        if (tags.get<bool>("ui-ignore", false)) return;

        if (m_mode == Mode::GENERATE) {
            Rml::Element* targetContainer = resolveTargetContainer(tags);
            if (targetContainer) {
                Rml::ElementPtr groupNode = m_doc->CreateElement("div");
                groupNode->SetClassNames("inspector-group " + tags.get<std::string>("ui-class", ""));
                
                Rml::ElementPtr titleNode = m_doc->CreateElement("div");
                titleNode->SetClassNames("inspector-group-title");
                titleNode->SetInnerRML(tags.get<std::string>("ui-label", name));
                groupNode->AppendChild(std::move(titleNode));

                Rml::Element* rawPtr = groupNode.get(); 
                targetContainer->AppendChild(std::move(groupNode));
                m_domStack.push_back(rawPtr);
            }
        }
        
        m_pathStack.push_back(name);
        value.inspect(*this);
        m_pathStack.pop_back();

        if (m_mode == Mode::GENERATE && !m_domStack.empty()) m_domStack.pop_back();
    }

    void onAction(const std::string& name, std::function<void()>& action, const PropertyTags& tags) override {
        if (m_mode == Mode::GENERATE) {
            std::string uiId = tags.get<std::string>("ui-id", getCurrentPath(name));
            Rml::Element* targetContainer = resolveTargetContainer(tags);
            if (!targetContainer) return;

            Rml::ElementPtr rowNode = m_doc->CreateElement("div");
            rowNode->SetClassNames("inspector-row action-row");

            Rml::ElementPtr btnNode = m_doc->CreateElement("button");
            btnNode->SetId(uiId);
            btnNode->SetClassNames("tool-btn " + tags.get<std::string>("ui-class", ""));
            btnNode->SetInnerRML(tags.get<std::string>("ui-label", name));

            if (action) {
                btnNode->AddEventListener(Rml::EventId::Click, new LambdaListener([action](Rml::Event&) { action(); }));
            }

            rowNode->AppendChild(std::move(btnNode));
            targetContainer->AppendChild(std::move(rowNode));
        }
    }

    void onBeginArray(const std::string& name, size_t& size, const PropertyTags& tags) override {
        if (m_mode == Mode::GENERATE) {
            Rml::Element* targetContainer = resolveTargetContainer(tags);
            if (targetContainer) {
                Rml::ElementPtr arrayNode = m_doc->CreateElement("div");
                arrayNode->SetClassNames("inspector-array " + tags.get<std::string>("ui-class", ""));

                Rml::ElementPtr titleNode = m_doc->CreateElement("h2");
                titleNode->SetClassNames("inspector-array-title");
                titleNode->SetInnerRML(tags.get<std::string>("ui-label", name));
                arrayNode->AppendChild(std::move(titleNode));

                Rml::Element* rawPtr = arrayNode.get();
                targetContainer->AppendChild(std::move(arrayNode));
                m_domStack.push_back(rawPtr);
            }
        }
        m_pathStack.push_back(name);
    }
    
    void onArrayElement(size_t) override {}
    
    void onEndArray() override {
        if (m_mode == Mode::GENERATE && !m_domStack.empty()) m_domStack.pop_back();
        m_pathStack.pop_back();
    }

    void onBeginPointer(const std::string& name, std::string& typeToken, bool& isNull, const PropertyTags& tags) override {
        if (m_mode == Mode::GENERATE) {
            Rml::Element* targetContainer = resolveTargetContainer(tags);
            if (targetContainer) {
                Rml::ElementPtr ptrNode = m_doc->CreateElement("div");
                ptrNode->SetClassNames("inspector-pointer " + tags.get<std::string>("ui-class", ""));
                
                Rml::Element* rawPtr = ptrNode.get();
                targetContainer->AppendChild(std::move(ptrNode));
                m_domStack.push_back(rawPtr);
            }
        }
        m_pathStack.push_back(name);
    }
    
    void onEndPointer() override {
        if (m_mode == Mode::GENERATE && !m_domStack.empty()) m_domStack.pop_back();
        m_pathStack.pop_back();
    }
};