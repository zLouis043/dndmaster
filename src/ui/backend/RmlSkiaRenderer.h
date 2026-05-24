#pragma once
#include <RmlUi/Core/RenderInterface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkImage.h>
#include <include/core/SkData.h>

class RmlSkiaRenderer : public Rml::RenderInterface {
public:
    void SetCanvas(SkCanvas* canvas) { currentCanvas = canvas; }

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                Rml::Span<const int> indices) override;
    
    void RenderGeometry(Rml::CompiledGeometryHandle handle, 
                        Rml::Vector2f translation, 
                        Rml::TextureHandle texture) override;
    
    void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture_handle) override;

private:
    SkCanvas* currentCanvas = nullptr;
    bool scissorEnabled = false;
    SkIRect scissorRect;
};