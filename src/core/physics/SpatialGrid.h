#pragma once
#include "../../data/elements/IMapElement.h"
#include <include/core/SkPoint.h>
#include <include/core/SkRect.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

class SpatialGrid {
public:
    float cellSize = 128.0f; 

private:
    std::unordered_map<int, std::vector<std::shared_ptr<IMapElement>>> m_cells;

    int getHash(int x, int y) const { 
        return x * 73856093 ^ y * 19349663; 
    }

    static SkRect computeAABB(const CollisionDescriptor& col) {
        if (col.points.empty()) return SkRect::MakeEmpty();
        SkRect rect = SkRect::MakeLTRB(col.points[0].x(), col.points[0].y(), col.points[0].x(), col.points[0].y());
        for (const auto& p : col.points) {
            rect.fLeft = std::min(rect.fLeft, p.x());
            rect.fRight = std::max(rect.fRight, p.x());
            rect.fTop = std::min(rect.fTop, p.y());
            rect.fBottom = std::max(rect.fBottom, p.y());
        }
        rect.outset(col.radiusOrThickness, col.radiusOrThickness);
        return rect;
    }

public:
    void clear() { 
        m_cells.clear(); 
    }

    void build(const std::vector<std::shared_ptr<IMapElement>>& elements) {
        clear();
        for (const auto& el : elements) {
            if (!el) continue;
            SkRect bounds = computeAABB(el->getCollider());
            
            int minX = std::floor(bounds.fLeft / cellSize);
            int maxX = std::floor(bounds.fRight / cellSize);
            int minY = std::floor(bounds.fTop / cellSize);
            int maxY = std::floor(bounds.fBottom / cellSize);

            for (int x = minX; x <= maxX; ++x) {
                for (int y = minY; y <= maxY; ++y) {
                    m_cells[getHash(x, y)].push_back(el);
                }
            }
        }
    }

    std::vector<std::shared_ptr<IMapElement>> query(const SkRect& queryBounds) const {
        std::unordered_set<std::shared_ptr<IMapElement>> uniqueElements;
        std::vector<std::shared_ptr<IMapElement>> result;

        int minX = std::floor(queryBounds.fLeft / cellSize);
        int maxX = std::floor(queryBounds.fRight / cellSize);
        int minY = std::floor(queryBounds.fTop / cellSize);
        int maxY = std::floor(queryBounds.fBottom / cellSize);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                auto it = m_cells.find(getHash(x, y));
                if (it != m_cells.end()) {
                    for (const auto& el : it->second) {
                        if (uniqueElements.insert(el).second) {
                            result.push_back(el);
                        }
                    }
                }
            }
        }
        return result;
    }
};