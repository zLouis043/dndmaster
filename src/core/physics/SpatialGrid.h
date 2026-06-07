#pragma once
#include "../ecs/entity.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <include/core/SkRect.h>

class SpatialGrid {
private:
    float m_cellSize;
    std::unordered_map<uint64_t, std::vector<EntityId>> m_cells;

    uint64_t getCellKey(int x, int y) const {
        return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(y);
    }

public:
    SpatialGrid(float cellSize = 100.0f) : m_cellSize(cellSize) {}
    void setCellSize(float size) { m_cellSize = size; }
    void clear() { m_cells.clear(); }

    void insert(EntityId id, const SkRect& aabb) {
        int minX = static_cast<int>(aabb.left() / m_cellSize);
        int minY = static_cast<int>(aabb.top() / m_cellSize);
        int maxX = static_cast<int>(aabb.right() / m_cellSize);
        int maxY = static_cast<int>(aabb.bottom() / m_cellSize);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                m_cells[getCellKey(x, y)].push_back(id);
            }
        }
    }

    void remove(EntityId id, const SkRect& oldAabb) {
        int minX = static_cast<int>(oldAabb.left() / m_cellSize);
        int minY = static_cast<int>(oldAabb.top() / m_cellSize);
        int maxX = static_cast<int>(oldAabb.right() / m_cellSize);
        int maxY = static_cast<int>(oldAabb.bottom() / m_cellSize);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                auto& cell = m_cells[getCellKey(x, y)];
                auto it = std::find(cell.begin(), cell.end(), id);
                if (it != cell.end()) cell.erase(it); 
            }
        }
    }

    void update(EntityId id, const SkRect& oldAabb, const SkRect& newAabb) {
        remove(id, oldAabb);
        insert(id, newAabb);
    }

    std::vector<EntityId> query(const SkRect& area) const {
        std::vector<EntityId> results;
        int minX = static_cast<int>(area.left() / m_cellSize);
        int minY = static_cast<int>(area.top() / m_cellSize);
        int maxX = static_cast<int>(area.right() / m_cellSize);
        int maxY = static_cast<int>(area.bottom() / m_cellSize);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                auto it = m_cells.find(getCellKey(x, y));
                if (it != m_cells.end()) {
                    results.insert(results.end(), it->second.begin(), it->second.end());
                }
            }
        }

        if (!results.empty()) {
            std::sort(results.begin(), results.end());
            results.erase(std::unique(results.begin(), results.end()), results.end());
        }
        
        return results;
    }
};