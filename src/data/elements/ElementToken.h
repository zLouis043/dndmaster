#pragma once
#include "IMapElement.h"
#include <include/core/SkColor.h>

class ElementToken : public IMapElement {
public:
    SkPoint pos = SkPoint::Make(0,0);
    float radius = 25.0f;
    SkColor color = SK_ColorMAGENTA; 

    ElementToken() : IMapElement("token") {}
    ElementToken(SkPoint p) : IMapElement("token"), pos(p) {}

    SkPoint getPivot() const override;
    void setPivot(SkPoint newPivot) override;

    void inspect(IInspector& inspector) override;
    std::vector<DrawDescriptor> getDrawDescriptors() const override;
    CollisionDescriptor getCollider() const override;
};