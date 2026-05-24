#pragma once
#include "../../core/engine.h"
#include "../../data/elements/IMapElement.h"
#include "../../data/entities/MapEntity.h"
#include <vector>
#include <memory>
#include <algorithm>

class CommandAddElement : public ICommand {
    MapEntity* m_map;
    std::vector<std::shared_ptr<IMapElement>> m_elements;
public:
    CommandAddElement(MapEntity* map, std::shared_ptr<IMapElement> newEl) 
        : m_map(map), m_elements({newEl}) {}
        
    CommandAddElement(MapEntity* map, const std::vector<std::shared_ptr<IMapElement>>& elements) 
        : m_map(map), m_elements(elements) {}
    
    void execute() override { 
        if (!m_map) return;
        for (auto& el : m_elements) m_map->elements.push_back(el);
        m_map->markGridDirty();
    }
    
    void undo() override { 
        if (!m_map) return;
        for (size_t i = 0; i < m_elements.size(); ++i) {
            if (!m_map->elements.empty()) m_map->elements.pop_back();
        }
        m_map->markGridDirty();
    }
};

class CommandRemoveElement : public ICommand {
    MapEntity* m_map;
    struct RemovedItem { size_t originalIndex; std::shared_ptr<IMapElement> el; };
    std::vector<RemovedItem> m_removed;

public:
    CommandRemoveElement(MapEntity* map, std::shared_ptr<IMapElement> el) : m_map(map) { init({el}); }
    CommandRemoveElement(MapEntity* map, const std::vector<std::shared_ptr<IMapElement>>& els) : m_map(map) { init(els); }

private:
    void init(const std::vector<std::shared_ptr<IMapElement>>& toRemove) {
        if (!m_map) return;
        for (const auto& target : toRemove) {
            auto it = std::find(m_map->elements.begin(), m_map->elements.end(), target);
            if (it != m_map->elements.end()) {
                m_removed.push_back({ (size_t)std::distance(m_map->elements.begin(), it), target });
            }
        }
        std::sort(m_removed.begin(), m_removed.end(), [](const RemovedItem& a, const RemovedItem& b) {
            return a.originalIndex > b.originalIndex;
        });
    }

public:
    void execute() override { 
        if (!m_map) return;
        for (const auto& item : m_removed) m_map->elements.erase(m_map->elements.begin() + item.originalIndex);
        m_map->markGridDirty();
        m_map->clearSelection(nullptr); 
    }
    
    void undo() override {
        if (!m_map) return;
        for (auto it = m_removed.rbegin(); it != m_removed.rend(); ++it) {
            m_map->elements.insert(m_map->elements.begin() + it->originalIndex, it->el);
        }
        m_map->markGridDirty();
    }
};

class CommandMoveElement : public ICommand {
    MapEntity* m_map;
    std::shared_ptr<IMapElement> m_el;
    SkPoint m_oldPivot;
    SkPoint m_newPivot;
public:
    CommandMoveElement(MapEntity* map, std::shared_ptr<IMapElement> el, SkPoint oldP, SkPoint newP) 
        : m_map(map), m_el(el), m_oldPivot(oldP), m_newPivot(newP) {}
        
    void execute() override { 
        m_el->setPivot(m_newPivot); 
        if(m_map) m_map->markGridDirty(); 
    }
    void undo() override { 
        m_el->setPivot(m_oldPivot); 
        if(m_map) m_map->markGridDirty(); 
    }
};