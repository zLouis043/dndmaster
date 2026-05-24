#include "ElementSegment.h"
#include "MapElementFactory.h"
#include "../plugs/SkiaPlugs.h" 

REGISTER_MAP_ELEMENT(ElementSegment, "segment")

SkPoint ElementSegment::getPivot() const { return p1; }

void ElementSegment::setPivot(SkPoint newPivot) { 
    SkPoint delta = newPivot - p1;
    p1 += delta; p2 += delta;
}

void ElementSegment::inspect(IInspector& inspector) {
    inspector.property("p1", p1).tag("ui-ignore", true);
    inspector.property("p2", p2).tag("ui-ignore", true);
    inspector.property("color", color).tag("ui-label", "Colore Muro").tag("ui-type", "color");
    inspector.property("thickness", thickness).tag("ui-label", "Spessore").tag("ui-type", "slider").tag("ui-min", 1.0f).tag("ui-max", 20.0f);
}

std::vector<DrawDescriptor> ElementSegment::getDrawDescriptors() const {
    DrawDescriptor d;
    d.type(DrawType::SEGMENT).property("p1", p1).property("p2", p2).property("color", (uint32_t)color).property("thickness", thickness);
    return { d };
}

CollisionDescriptor ElementSegment::getCollider() const {
    CollisionDescriptor c; c.type = ColliderType::SEGMENT; c.points = {p1, p2}; c.radiusOrThickness = thickness;
    return c;
}