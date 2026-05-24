#pragma once
#include "IMapElement.h"
#include <include/core/SkColor.h>

class ElementSegment : public IMapElement {
public:
    SkPoint p1 = SkPoint::Make(0,0);
    SkPoint p2 = SkPoint::Make(0,0);
    SkColor color = SK_ColorCYAN;
    float thickness = 4.0f;

    ElementSegment() : IMapElement("segment") {}
    ElementSegment(SkPoint start, SkPoint end) : IMapElement("segment"), p1(start), p2(end) {}

    SkPoint getPivot() const override;
    void setPivot(SkPoint newPivot) override;

    void inspect(IInspector& inspector) override;
    std::vector<DrawDescriptor> getDrawDescriptors() const override;
    CollisionDescriptor getCollider() const override;
};