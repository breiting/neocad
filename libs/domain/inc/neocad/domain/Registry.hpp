#pragma once
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <unordered_map>

namespace nc::domain {

class Registry {
   public:
    Entity CreateEntity();

    template <typename T>
    void AddComponent(Entity e, const T& comp);

    template <typename T>
    T* GetComponent(Entity e);

    template <typename T>
    bool HasComponent(Entity e) const;

    std::vector<Entity> Entities() const;

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
};
}  // namespace nc::domain
