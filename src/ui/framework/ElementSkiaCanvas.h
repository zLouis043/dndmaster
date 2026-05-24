#pragma once
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementInstancer.h>
#include <functional>

class Engine;
class SkCanvas;

class ElementSkiaCanvas : public Rml::Element {
public:
    ElementSkiaCanvas(const Rml::String& tag, Engine* engine);
    virtual ~ElementSkiaCanvas() = default;

    using DrawCallback = std::function<void(SkCanvas*, float width, float height)>;
    
    void SetDrawCallback(DrawCallback callback) { m_drawCallback = std::move(callback); }

protected:
    void OnRender() override;

private:
    Engine* m_engine;
    DrawCallback m_drawCallback;
};

class InstancerSkiaCanvas : public Rml::ElementInstancer {
public:
    InstancerSkiaCanvas(Engine* engine) : m_engine(engine) {}

    Rml::ElementPtr InstanceElement(Rml::Element* parent, const Rml::String& tag, const Rml::XMLAttributes& attributes) override {
        return Rml::ElementPtr(new ElementSkiaCanvas(tag, m_engine));
    }

    void ReleaseElement(Rml::Element* element) override {
        delete element;
    }

private:
    Engine* m_engine;
};