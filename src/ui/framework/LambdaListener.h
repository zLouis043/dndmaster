#pragma once
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/Element.h>
#include <functional>

class LambdaListener : public Rml::EventListener {
public:
    LambdaListener(std::function<void(Rml::Event&)> callback) 
        : m_callback(std::move(callback)) {}

    void ProcessEvent(Rml::Event& event) override {
        if (m_callback) m_callback(event);
    }

    void OnDetach(Rml::Element* element) override {
        delete this; 
    }

private:
    std::function<void(Rml::Event&)> m_callback;
};