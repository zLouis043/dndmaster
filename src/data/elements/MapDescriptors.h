#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <include/core/SkPoint.h>

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
};