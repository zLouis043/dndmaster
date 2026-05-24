#pragma once
#include "../../data/elements/IMapElement.h"
#include "SpatialGrid.h"
#include <include/core/SkPoint.h>
#include <vector>
#include <memory>

class CollisionEngine {
public:
    static std::shared_ptr<IMapElement> hitTest(const SpatialGrid& grid, const std::vector<std::shared_ptr<IMapElement>>& allElements, SkPoint mousePos, float threshold) {
        std::vector<std::shared_ptr<IMapElement>> candidates = allElements;
        
        if (allElements.size() > 50) {
            SkRect queryRect = SkRect::MakeLTRB(mousePos.x() - threshold, mousePos.y() - threshold, mousePos.x() + threshold, mousePos.y() + threshold);
            candidates = grid.query(queryRect);
        }

        for (auto it = candidates.rbegin(); it != candidates.rend(); ++it) {
            if (checkCollision((*it)->getCollider(), (*it).get(), mousePos, threshold)) return *it;
        }
        return nullptr;
    }

    static std::vector<std::shared_ptr<IMapElement>> hitTestMulti(const SpatialGrid& grid, const std::vector<std::shared_ptr<IMapElement>>& allElements, SkPoint mousePos, float radius) {
        std::vector<std::shared_ptr<IMapElement>> hits;
        std::vector<std::shared_ptr<IMapElement>> candidates = allElements;

        if (allElements.size() > 50) {
            SkRect queryRect = SkRect::MakeLTRB(mousePos.x() - radius, mousePos.y() - radius, mousePos.x() + radius, mousePos.y() + radius);
            candidates = grid.query(queryRect);
        }

        for (const auto& el : candidates) {
            if (checkCollision(el->getCollider(), el.get(), mousePos, radius)) {
                hits.push_back(el);
            }
        }
        return hits;
    }

private:
    static bool checkCollision(const CollisionDescriptor& col, const IMapElement* element, SkPoint point, float threshold) {
        switch (col.type) {
            case ColliderType::CIRCLE: {
                if (col.points.empty()) return false;
                return SkPoint::Distance(point, col.points[0]) <= (col.radiusOrThickness + threshold);
            }
            case ColliderType::SEGMENT: {
                if (col.points.size() < 2) return false;
                SkPoint p1 = col.points[0]; SkPoint p2 = col.points[1];
                float l2 = SkPoint::Distance(p1, p2) * SkPoint::Distance(p1, p2);
                if (l2 == 0.0f) return SkPoint::Distance(point, p1) < (threshold + col.radiusOrThickness);
                float t = ((point.x() - p1.x()) * (p2.x() - p1.x()) + (point.y() - p1.y()) * (p2.y() - p1.y())) / l2;
                t = std::max(0.0f, std::min(1.0f, t));
                SkPoint proj = SkPoint::Make(p1.x() + t * (p2.x() - p1.x()), p1.y() + t * (p2.y() - p1.y()));
                return SkPoint::Distance(point, proj) <= (threshold + col.radiusOrThickness);
            }
            case ColliderType::CUSTOM: return element->customHitTest(point, threshold);
            default: return false;
        }
    }
};