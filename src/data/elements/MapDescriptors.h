#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <include/core/SkPoint.h>
#include <include/core/SkRect.h>

enum class DrawType { CIRCLE, SEGMENT, PATH, RECT };
enum class ColliderType { CIRCLE, SEGMENT, PATH, CUSTOM };

class DrawDescriptor {
public:
    DrawType shapeType;
    using PropValue = std::variant<float, int, uint32_t, SkPoint, std::vector<SkPoint>>;
    std::unordered_map<std::string, PropValue> props;

    DrawDescriptor& type(DrawType t) { shapeType = t; return *this; }
    template <typename T> DrawDescriptor& property(const std::string& key, const T& value) { props[key] = value; return *this; }
    
    template <typename T> T get(const std::string& key, T fallback) const {
        auto it = props.find(key);
        if (it != props.end() && std::holds_alternative<T>(it->second)) return std::get<T>(it->second);
        return fallback;
    }
};

class CollisionDescriptor {
public:
    ColliderType type;
    std::vector<SkPoint> points; 
    float radiusOrThickness = 0.0f;

    SkRect getBounds() const {
        if (points.empty()) {
            return SkRect::MakeEmpty();
        }

        float minX = points[0].fX;
        float minY = points[0].fY;
        float maxX = points[0].fX;
        float maxY = points[0].fY;

        for (size_t i = 1; i < points.size(); ++i) {
            if (points[i].fX < minX) minX = points[i].fX;
            if (points[i].fY < minY) minY = points[i].fY;
            if (points[i].fX > maxX) maxX = points[i].fX;
            if (points[i].fY > maxY) maxY = points[i].fY;
        }

        minX -= radiusOrThickness;
        minY -= radiusOrThickness;
        maxX += radiusOrThickness;
        maxY += radiusOrThickness;

        return SkRect::MakeLTRB(minX, minY, maxX, maxY);
    }
};