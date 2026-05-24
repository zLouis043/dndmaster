#include "ElementToken.h"
#include "MapElementFactory.h"
#include "../plugs/SkiaPlugs.h"

REGISTER_MAP_ELEMENT(ElementToken, "token")

SkPoint ElementToken::getPivot() const { return pos; }
void ElementToken::setPivot(SkPoint newPivot) { pos = newPivot; }

void ElementToken::inspect(IInspector& inspector) {
    inspector.property("pos", pos).tag("ui-ignore", true);
    inspector.property("radius", radius).tag("ui-label", "Raggio (px)").tag("ui-type", "slider").tag("ui-min", 10.0f).tag("ui-max", 150.0f);
    inspector.property("color", color).tag("ui-label", "Colore Base").tag("ui-type", "color");
}

std::vector<DrawDescriptor> ElementToken::getDrawDescriptors() const {
    DrawDescriptor d;
    d.type(DrawType::CIRCLE).property("center", pos).property("radius", radius).property("inner-color", (uint32_t)color).property("outer-color", (uint32_t)SK_ColorBLACK).property("stroke-width", 2.0f);
    return { d };
}

CollisionDescriptor ElementToken::getCollider() const {
    CollisionDescriptor c; c.type = ColliderType::CIRCLE; c.points = {pos}; c.radiusOrThickness = radius;
    return c;
}