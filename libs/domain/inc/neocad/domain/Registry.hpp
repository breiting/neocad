#pragma once

#include <functional>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace nc::domain {

class Registry {
   public:
    using Callback = std::function<void(Entity)>;

    // --------- ENTITY ----------
    Entity CreateEntity() {
        return m_NextId++;
    }

    // --------- COMPONENT CORE API ----------

    template <typename T>
    void AddComponent(Entity e, const T& comp) {
        auto& map = Storage<T>();
        map[e] = comp;
        TriggerAdded<T>(e);
    }

    template <typename T>
    void RemoveComponent(Entity e) {
        auto& map = Storage<T>();
        map.erase(e);
        TriggerRemoved<T>(e);
    }

    template <typename T>
    T* GetComponent(Entity e) {
        auto& map = Storage<T>();
        auto it = map.find(e);
        return (it != map.end()) ? &it->second : nullptr;
    }

    template <typename T>
    const T* GetComponent(Entity e) const {
        const auto& map = StorageConst<T>();
        auto it = map.find(e);
        return (it != map.end()) ? &it->second : nullptr;
    }

    template <typename T>
    bool HasComponent(Entity e) const {
        const auto& map = StorageConst<T>();
        return map.find(e) != map.end();
    }

    // --------- ENTITIES ----------
    std::vector<Entity> Entities() const;

    // --------- EVENTS ----------
    template <typename T>
    void OnComponentAdded(Callback cb) {
        m_OnAdded[std::type_index(typeid(T))].push_back(cb);
    }

    template <typename T>
    void OnComponentRemoved(Callback cb) {
        m_OnRemoved[std::type_index(typeid(T))].push_back(cb);
    }

    template <typename T>
    void OnComponentModified(Callback cb) {
        m_OnModified[std::type_index(typeid(T))].push_back(cb);
    }

    // --------- DEBUG ----------
    void Dump() const;

   private:
    Entity m_NextId = 1;

    // Events keyed by component type
    std::unordered_map<std::type_index, std::vector<Callback>> m_OnAdded;
    std::unordered_map<std::type_index, std::vector<Callback>> m_OnRemoved;
    std::unordered_map<std::type_index, std::vector<Callback>> m_OnModified;

    // --------- STORAGE PER COMPONENT-TYPE ----------
    template <typename T>
    using MapT = std::unordered_map<Entity, T>;

    template <typename T>
    static MapT<T>& Storage() {
        static MapT<T> s_Map;
        return s_Map;
    }

    template <typename T>
    static const MapT<T>& StorageConst() {
        return Storage<T>();
    }

    // --------- EVENT HELPERS ----------
    template <typename T>
    void TriggerAdded(Entity e) {
        auto it = m_OnAdded.find(std::type_index(typeid(T)));
        if (it == m_OnAdded.end())
            return;
        for (auto& cb : it->second)
            cb(e);
    }

    template <typename T>
    void TriggerRemoved(Entity e) {
        auto it = m_OnRemoved.find(std::type_index(typeid(T)));
        if (it == m_OnRemoved.end())
            return;
        for (auto& cb : it->second)
            cb(e);
    }

    template <typename T>
    void TriggerModified(Entity e) {
        auto it = m_OnModified.find(std::type_index(typeid(T)));
        if (it == m_OnModified.end())
            return;
        for (auto& cb : it->second)
            cb(e);
    }
};

}  // namespace nc::domain
