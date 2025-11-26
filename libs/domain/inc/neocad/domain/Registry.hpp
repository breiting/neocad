#pragma once
#include <functional>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <unordered_map>

namespace nc::domain {

class Registry {
   public:
    using Callback = std::function<void(Entity)>;

    Entity CreateEntity();

    template <typename T>
    void AddComponent(Entity e, const T& comp);

    template <typename T>
    void RemoveComponent(Entity e);

    template <typename T>
    T* GetComponent(Entity e);

    template <typename T>
    bool HasComponent(Entity e) const;

    std::vector<Entity> Entities() const;

    // EVENT API
    template <typename T>
    void OnComponentAdded(Callback cb) {
        m_OnAdded[typeid(T).hash_code()].push_back(cb);
    }

    template <typename T>
    void OnComponentRemoved(Callback cb) {
        m_OnRemoved[typeid(T).hash_code()].push_back(cb);
    }

    template <typename T>
    void OnComponentModified(Callback cb) {
        m_OnModified[typeid(T).hash_code()].push_back(cb);
    }

   private:
    template <typename T>
    void TriggerAdded(Entity e) {
        auto it = m_OnAdded.find(typeid(T).hash_code());
        if (it != m_OnAdded.end())
            for (auto& fn : it->second)
                fn(e);
    }
    template <typename T>
    void TriggerRemoved(Entity e) {
        auto it = m_OnRemoved.find(typeid(T).hash_code());
        if (it != m_OnRemoved.end())
            for (auto& fn : it->second)
                fn(e);
    }
    template <typename T>
    void TriggerModified(Entity e) {
        auto it = m_OnModified.find(typeid(T).hash_code());
        if (it != m_OnModified.end())
            for (auto& fn : it->second)
                fn(e);
    }

   private:
    Entity m_NextId = 1;
    std::unordered_map<Entity, NameComponent> m_Names;
    std::unordered_map<Entity, PositionComponent> m_Positions;
    std::unordered_map<Entity, RadiusComponent> m_Circles;
    std::unordered_map<Entity, EdgeComponent> m_Lines;
    std::unordered_map<Entity, FaceComponent> m_Faces;
    std::unordered_map<Entity, BodyComponent> m_Bodies;
    std::unordered_map<Entity, MeshComponent> m_Meshes;
    std::unordered_map<Entity, SketchPlaneComponent> m_SketchPlanes;

    // Event callbacks
    std::unordered_map<size_t, std::vector<Callback>> m_OnAdded;
    std::unordered_map<size_t, std::vector<Callback>> m_OnRemoved;
    std::unordered_map<size_t, std::vector<Callback>> m_OnModified;
};
}  // namespace nc::domain
