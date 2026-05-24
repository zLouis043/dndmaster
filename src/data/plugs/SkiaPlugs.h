#pragma once
#include "../inspector/Inspection.h"
#include <include/core/SkPoint.h>

template <>
struct TypeCodec<SkPoint> {
    static void write(IInspector& ins, const std::string& name, SkPoint& value, const PropertyTags& tags) {
        float x = value.x(), y = value.y();
        ins.property(name + "_x", x).tags(tags);
        ins.property(name + "_y", y).tags(tags);
    }
    static void read(IInspector& ins, const std::string& name, SkPoint& value, const PropertyTags& tags) {
        float x = 0, y = 0;
        ins.property(name + "_x", x).tags(tags);
        ins.property(name + "_y", y).tags(tags);
        value.set(x, y);
    }
};