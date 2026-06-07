#pragma once
#include "SpatialGrid.h"
#include "../ecs/World.h"
#include "../../data/elements/IMapElement.h"
#include <include/core/SkPoint.h>
#include <include/core/SkRect.h>
#include <vector>
#include <algorithm>
#include <cmath>

class CollisionEngine {
private:
    static float distancePointToSegment(SkPoint pt, SkPoint p1, SkPoint p2) {
        float dx = p2.fX - p1.fX;
        float dy = p2.fY - p1.fY;
        float l2 = dx * dx + dy * dy;
        
        if (l2 == 0.0f) {
            float dx0 = pt.fX - p1.fX;
            float dy0 = pt.fY - p1.fY;
            return std::sqrt(dx0 * dx0 + dy0 * dy0);
        }
        
        float t = ((pt.fX - p1.fX) * dx + (pt.fY - p1.fY) * dy) / l2;
        t = std::max(0.0f, std::min(1.0f, t));
        
        float projX = p1.fX + t * dx;
        float projY = p1.fY + t * dy;
        
        float dxProj = pt.fX - projX;
        float dyProj = pt.fY - projY;
        return std::sqrt(dxProj * dxProj + dyProj * dyProj);
    }

    static bool checkCollision(SkPoint queryCenter, float queryRadius, const CollisionDescriptor& desc) {
        if (desc.points.empty()) return false;

        if (desc.type == ColliderType::CIRCLE) {
            float dx = queryCenter.fX - desc.points[0].fX;
            float dy = queryCenter.fY - desc.points[0].fY;
            float dist = std::sqrt(dx * dx + dy * dy);
            return dist <= (queryRadius + desc.radiusOrThickness);
        } 
        else if (desc.type == ColliderType::SEGMENT) {
            if (desc.points.size() < 2) return false;
            float dist = distancePointToSegment(queryCenter, desc.points[0], desc.points[1]);
            return dist <= (queryRadius + (desc.radiusOrThickness / 2.0f));
        }
        
        return false;
    }

public:
    static EntityId hitTest(const SpatialGrid& grid, const World<IMapElement>& world, SkPoint pt) {
        float clickRadius = 2.0f;
        SkRect queryRect = SkRect::MakeLTRB(pt.fX - clickRadius, pt.fY - clickRadius, 
                                            pt.fX + clickRadius, pt.fY + clickRadius);
                                            
        std::vector<EntityId> candidates = grid.query(queryRect);
        EntityId hitId = 0;
        
        world.queryReverseHit([&](EntityId id, IMapElement& el) {
            if (std::find(candidates.begin(), candidates.end(), id) == candidates.end()) return false;
            
            if (checkCollision(pt, clickRadius, el.getCollider())) {
                hitId = id;
                return true; 
            }
            return false;
        });
        
        return hitId;
    }

    static std::vector<EntityId> hitTestMulti(const SpatialGrid& grid, const World<IMapElement>& world, SkPoint pt, float radius) {
        SkRect queryRect = SkRect::MakeLTRB(pt.fX - radius, pt.fY - radius, 
                                            pt.fX + radius, pt.fY + radius);
        std::vector<EntityId> candidates = grid.query(queryRect);
        std::vector<EntityId> results;
        
        for (EntityId id : candidates) {
            if (world.has(id)) {
                if (checkCollision(pt, radius, world.get(id).getCollider())) {
                    results.push_back(id);
                }
            }
        }
        return results;
    }
};