#pragma once
#include "entity.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

template <typename T>
class World {
private:
    EntityId m_nextId = 1;
    std::unordered_map<EntityId, std::shared_ptr<T>> m_payloads;
    std::vector<EntityId> m_activeEntities;

public:
    template <typename ConcreteType, typename... Args>
    EntityId create(Args&&... args) {
        EntityId id = m_nextId++;
        m_payloads[id] = std::make_shared<ConcreteType>(std::forward<Args>(args)...);
        m_activeEntities.push_back(id);
        return id;
    }

    EntityId insert(std::shared_ptr<T> payload) {
        EntityId id = m_nextId++;
        m_payloads[id] = std::move(payload);
        m_activeEntities.push_back(id);
        return id;
    }

    std::vector<EntityId> fill(const std::vector<std::shared_ptr<T>>& entities){
        std::vector<EntityId> ids;
        for(auto& e : entities){
            ids.push_back(insert(e));
        }
        return ids;
    }

    const std::vector<std::shared_ptr<T>> flatten(){
        std::vector<std::shared_ptr<T>> entities;
        for(auto& id : m_activeEntities){
            entities.push_back(getShared(id));
        }
        return entities;
    } 

    void restore(EntityId id, std::shared_ptr<T>&& payload) {
        m_payloads[id] = std::move(payload);
        m_activeEntities.push_back(id);
        if (id >= m_nextId) m_nextId = id + 1;
    }

    std::shared_ptr<T> extract(EntityId id) {
        auto it = m_payloads.find(id);
        if (it == m_payloads.end()) return nullptr;
        
        auto payload = it->second;
        m_payloads.erase(it);
        auto vecIt = std::find(m_activeEntities.begin(), m_activeEntities.end(), id);
        if (vecIt != m_activeEntities.end()) m_activeEntities.erase(vecIt);
        
        return payload;
    }

    void destroy(EntityId id) { extract(id); }

    T& get(EntityId id) {
    auto it = m_payloads.find(id);
    if (it == m_payloads.end()) {
        throw std::runtime_error("[WORLD] get(): EntityId " + std::to_string(id) + " does not exist.");
    }
    return *(it->second);
}

const T& get(EntityId id) const {
    auto it = m_payloads.find(id);
    if (it == m_payloads.end()) {
        throw std::runtime_error("[WORLD] get(): EntityId " + std::to_string(id) + " does not exist.");
    }
    return *(it->second);
}

    std::shared_ptr<T> getShared(EntityId id) const {
        auto it = m_payloads.find(id);
        return (it != m_payloads.end()) ? it->second : nullptr;
    }

    const std::vector<EntityId>& getActiveIds() const { return m_activeEntities; }
    
    bool has(EntityId id) const { return m_payloads.count(id) > 0; }
    void clear() { m_payloads.clear(); m_activeEntities.clear(); m_nextId = 1; }

    template <typename Func>
    void forEach(Func f) const {
        for (EntityId id : m_activeEntities) f(id, *(m_payloads.at(id)));
    }

    template <typename Func>
    bool queryReverseHit(Func f) const {
        for (auto it = m_activeEntities.rbegin(); it != m_activeEntities.rend(); ++it) {
            if (f(*it, *(m_payloads.at(*it)))) return true;
        }
        return false;
    }

    template <typename CompareFunc>
    void sort(CompareFunc comp) {
        std::sort(m_activeEntities.begin(), m_activeEntities.end(), 
            [&](EntityId a, EntityId b) { return comp(a, *m_payloads[a], b, *m_payloads[b]); }
        );
    }
};