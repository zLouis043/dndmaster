#pragma once
#include "../../core/engine.h"
#include "../../data/elements/IMapElement.h"
#include "../../data/entities/MapEntity.h"
#include "../../core/ecs/entity.h"
#include <vector>
#include <memory>
#include <algorithm>

class CommandAddElement : public ICommand {
    MapEntity* m_map;
    std::vector<std::shared_ptr<IMapElement>> m_payloads;
    std::vector<EntityId> m_assignedIds;

public:
    CommandAddElement(MapEntity* map, std::shared_ptr<IMapElement>&& newEl) 
        : m_map(map) { m_payloads.push_back(std::move(newEl)); }
        
    CommandAddElement(MapEntity* map, std::vector<std::shared_ptr<IMapElement>>&& elements) 
        : m_map(map), m_payloads(std::move(elements)) {}
    
    void execute() override { 
        if (!m_map || m_payloads.empty()) return;
        
        bool isRedo = !m_assignedIds.empty();
        
        for (size_t i = 0; i < m_payloads.size(); ++i) {
            SkRect bounds = m_payloads[i]->getCollider().getBounds();
            EntityId id;
            
            if (isRedo) {
                id = m_assignedIds[i];
                m_map->world.restore(id, std::move(m_payloads[i]));
            } else {
                id = m_map->world.insert(std::move(m_payloads[i]));
                m_assignedIds.push_back(id);
            }
            m_map->grid.insert(id, bounds);
        }
    }
    
    void undo() override { 
        if (!m_map) return;
        m_payloads.clear();
        for (EntityId id : m_assignedIds) {
            auto payload = m_map->world.extract(id);
            m_map->grid.remove(id, payload->getCollider().getBounds());
            m_payloads.push_back(std::move(payload));
            
            if (m_map->selectedEntityId == id) m_map->clearSelection(nullptr);
        }
    }
};

class CommandRemoveElement : public ICommand {
    MapEntity* m_map;
    std::vector<EntityId> m_targetIds;
    
    struct SavedItem { EntityId id; std::shared_ptr<IMapElement> payload; SkRect bounds; };
    std::vector<SavedItem> m_saved;

public:
    CommandRemoveElement(MapEntity* map, EntityId id) : m_map(map), m_targetIds({id}) {}
    CommandRemoveElement(MapEntity* map, const std::vector<EntityId>& ids) : m_map(map), m_targetIds(ids) {}

    void execute() override { 
        if (!m_map) return;
        m_saved.clear();
        
        for (EntityId id : m_targetIds) {
            if (!m_map->world.has(id)) continue;
            
            SkRect bounds = m_map->world.get(id).getCollider().getBounds();
            auto payload = m_map->world.extract(id);
            m_map->grid.remove(id, bounds);
            
            m_saved.push_back({id, std::move(payload), bounds});
            
            if (m_map->selectedEntityId == id) m_map->clearSelection(nullptr);
        }
    }
    
    void undo() override {
        if (!m_map) return;
        for (auto& item : m_saved) {
            m_map->world.restore(item.id, std::move(item.payload));
            m_map->grid.insert(item.id, item.bounds);
        }
        m_saved.clear();
    }
};

class CommandMoveElement : public ICommand {
    MapEntity* m_map;
    EntityId m_targetId;
    SkPoint m_oldPivot;
    SkPoint m_newPivot;

public:
    CommandMoveElement(MapEntity* map, EntityId id, SkPoint oldP, SkPoint newP) 
        : m_map(map), m_targetId(id), m_oldPivot(oldP), m_newPivot(newP) {}
        
    void execute() override { 
        if (!m_map || !m_map->world.has(m_targetId)) return;
        auto& el = m_map->world.get(m_targetId);
        
        SkRect oldAabb = el.getCollider().getBounds();
        el.setPivot(m_newPivot); 
        SkRect newAabb = el.getCollider().getBounds();
        
        m_map->grid.update(m_targetId, oldAabb, newAabb); 
    }

    void undo() override { 
        if (!m_map || !m_map->world.has(m_targetId)) return;
        auto& el = m_map->world.get(m_targetId);
        
        SkRect oldAabb = el.getCollider().getBounds();
        el.setPivot(m_oldPivot); 
        SkRect newAabb = el.getCollider().getBounds();
        
        m_map->grid.update(m_targetId, oldAabb, newAabb); 
    }
};