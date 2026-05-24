#include "RmlSkiaRenderer.h"
#include <include/core/SkVertices.h>
#include <include/core/SkPaint.h>
#include <include/core/SkShader.h>
#include <vector>

Rml::CompiledGeometryHandle RmlSkiaRenderer::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) {
    int num_vertices = static_cast<int>(vertices.size());
    int num_indices = static_cast<int>(indices.size());

    std::vector<SkPoint> positions(num_vertices);
    std::vector<SkColor> colors(num_vertices);
    std::vector<SkPoint> texCoords(num_vertices);
    std::vector<uint16_t> sk_indices(num_indices);

    for (int i = 0; i < num_vertices; ++i) {
        positions[i] = SkPoint::Make(vertices[i].position.x, vertices[i].position.y);
        colors[i] = SkColorSetARGB(vertices[i].colour.alpha, vertices[i].colour.red,
                                   vertices[i].colour.green, vertices[i].colour.blue);
        texCoords[i] = SkPoint::Make(vertices[i].tex_coord.x, vertices[i].tex_coord.y);
    }

    for (int i = 0; i < num_indices; ++i) {
        sk_indices[i] = static_cast<uint16_t>(indices[i]);
    }

    auto skVertices = SkVertices::MakeCopy(SkVertices::kTriangles_VertexMode,
                                           num_vertices, positions.data(), texCoords.data(), colors.data(),
                                           num_indices, sk_indices.data());

    sk_sp<SkVertices>* handle = new sk_sp<SkVertices>(skVertices);
    return reinterpret_cast<Rml::CompiledGeometryHandle>(handle);
}

void RmlSkiaRenderer::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) {
    if (!currentCanvas || !handle) return;
    
    sk_sp<SkVertices>* skVertices = reinterpret_cast<sk_sp<SkVertices>*>(handle);

    SkPaint paint;
    paint.setAntiAlias(false); 
    
    paint.setColor(SK_ColorWHITE); 

    if (texture) {
        sk_sp<SkImage>* img = reinterpret_cast<sk_sp<SkImage>*>(texture);
        SkMatrix localMatrix;
        localMatrix.setScale(1.0f / (*img)->width(), 1.0f / (*img)->height());
        
        paint.setShader((*img)->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(SkFilterMode::kLinear), &localMatrix));
    }

    currentCanvas->save();
    
    if (scissorEnabled) {
        currentCanvas->clipIRect(scissorRect);
    }
    
    currentCanvas->translate(std::round(translation.x), std::round(translation.y));
    
    currentCanvas->drawVertices(skVertices->get(), SkBlendMode::kModulate, paint);
    
    currentCanvas->restore();
}

void RmlSkiaRenderer::ReleaseGeometry(Rml::CompiledGeometryHandle geometry) {
    if (geometry) {
        sk_sp<SkVertices>* handle = reinterpret_cast<sk_sp<SkVertices>*>(geometry);
        delete handle;
    }
}

void RmlSkiaRenderer::EnableScissorRegion(bool enable) {
    scissorEnabled = enable;
}

void RmlSkiaRenderer::SetScissorRegion(Rml::Rectanglei region) {
    scissorRect = SkIRect::MakeXYWH(region.Left(), region.Top(), region.Width(), region.Height());
}

Rml::TextureHandle RmlSkiaRenderer::GenerateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions) {
    
    std::vector<uint8_t> clean_data(source_data.begin(), source_data.end());
    
    for(size_t i = 0; i < clean_data.size(); i += 4) {
        clean_data[i]   = 255;
        clean_data[i+1] = 255;
        clean_data[i+2] = 255;
    }

    SkImageInfo info = SkImageInfo::Make(source_dimensions.x, source_dimensions.y, kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
    sk_sp<SkData> data = SkData::MakeWithCopy(clean_data.data(), clean_data.size());
    sk_sp<SkImage> image = SkImages::RasterFromData(info, data, source_dimensions.x * 4);
    
    if (!image) return 0;
    
    sk_sp<SkImage>* handle = new sk_sp<SkImage>(image);
    return reinterpret_cast<Rml::TextureHandle>(handle);
}

Rml::TextureHandle RmlSkiaRenderer::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) {
    return 0; 
}

void RmlSkiaRenderer::ReleaseTexture(Rml::TextureHandle texture_handle) {
    if (texture_handle) {
        sk_sp<SkImage>* img = reinterpret_cast<sk_sp<SkImage>*>(texture_handle);
        delete img;
    }
}