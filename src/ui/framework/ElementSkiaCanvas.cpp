#include "ElementSkiaCanvas.h"
#include "../../core/engine.h"
#include <include/core/SkCanvas.h>
#include <include/core/SkRect.h>

ElementSkiaCanvas::ElementSkiaCanvas(const Rml::String& tag, Engine* engine) 
    : Rml::Element(tag), m_engine(engine) {}

void ElementSkiaCanvas::OnRender() {
    if (!m_drawCallback || !m_engine) return;

    SkCanvas* skCanvas = m_engine->getCanvas();
    if (!skCanvas) return;

    Rml::Vector2f position = GetAbsoluteOffset();
    Rml::Vector2f size = GetBox().GetSize();

    skCanvas->save();
    
    skCanvas->translate(position.x, position.y);

    SkRect clipRect = SkRect::MakeWH(size.x, size.y);
    skCanvas->clipRect(clipRect);

    m_drawCallback(skCanvas, size.x, size.y);

    skCanvas->restore();
}