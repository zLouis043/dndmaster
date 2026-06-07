#pragma once
#include "../../data/elements/IMapElement.h"
#include "../../core/ecs/World.h"
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <vector>
#include <memory>

class MapRenderingEngine {
public:
    static void render(SkCanvas* canvas, const World<IMapElement>& world, float zoom) {
        world.forEach([&](EntityId id, IMapElement& element) {
            auto descriptors = element.getDrawDescriptors();
            for (const auto& desc : descriptors) {
                renderDescriptor(canvas, desc, zoom);
            }
        });
    }

private:
    static void renderDescriptor(SkCanvas* canvas, const DrawDescriptor& desc, float zoom) {
        SkPaint paint;
        paint.setAntiAlias(true);

        uint32_t innerColor = desc.get<uint32_t>("inner-color", 0x00000000);
        uint32_t outerColor = desc.get<uint32_t>("outer-color", 0x00000000); 
        float strokeWidth   = desc.get<float>("stroke-width", 1.0f);

        switch (desc.shapeType) {
            case DrawType::CIRCLE: {
                SkPoint center = desc.get<SkPoint>("center", SkPoint::Make(0,0));
                float radius = desc.get<float>("radius", 10.0f);

                if (innerColor != 0x00000000) {
                    paint.setStyle(SkPaint::kFill_Style);
                    paint.setColor(innerColor);
                    canvas->drawCircle(center, radius, paint);
                }

                if (outerColor != 0x00000000) {
                    paint.setStyle(SkPaint::kStroke_Style);
                    paint.setColor(outerColor);
                    paint.setStrokeWidth(strokeWidth / zoom);
                    canvas->drawCircle(center, radius, paint);
                }
                break;
            }

            case DrawType::SEGMENT: {
                SkPoint p1 = desc.get<SkPoint>("p1", SkPoint::Make(0,0));
                SkPoint p2 = desc.get<SkPoint>("p2", SkPoint::Make(0,0));

                uint32_t color = desc.get<uint32_t>("color", innerColor);
                float thickness = desc.get<float>("thickness", strokeWidth);

                paint.setStyle(SkPaint::kStroke_Style);
                paint.setColor(color);
                paint.setStrokeWidth(thickness / zoom);
                canvas->drawLine(p1, p2, paint);
                break;
            }

            case DrawType::PATH:
            case DrawType::RECT:
                break; 
        }
    }
};