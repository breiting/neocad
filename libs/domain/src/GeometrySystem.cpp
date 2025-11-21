#include <neocad/domain/GeometrySystem.hpp>

#include "neocad/domain/Components.hpp"
#include "neocad/domain/Entity.hpp"

namespace nc {

GeometrySystem::GeometrySystem(Registry& reg, IGeometryBackend& backend) : m_Registry(reg), m_Backend(backend) {
}

// ---------- CREATE ----------

Entity GeometrySystem::CreatePoint(const vec3& pos) {
    Entity e = m_Registry.CreateEntity();
    m_Registry.AddComponent<PositionComponent>(e, PositionComponent{pos});
    return e;
}

Entity GeometrySystem::CreateLine(Entity p1, Entity p2) {
    if (!m_Registry.HasComponent<PositionComponent>(p1) || !m_Registry.HasComponent<PositionComponent>(p2))
        return INVALID_ENTITY;

    Entity e = m_Registry.CreateEntity();
    m_Registry.AddComponent<LineComponent>(e, LineComponent{p1, p2});
    return e;
}

Entity GeometrySystem::CreateFace(const std::vector<Entity>& edges) {
    Entity e = m_Registry.CreateEntity();
    m_Registry.AddComponent<FaceComponent>(e, FaceComponent{edges});
    return e;
}

// ---------- EXTRUDE ----------

Entity GeometrySystem::ExtrudeFace(Entity face, double height) {
    Polygon poly;
    if (!BuildPolygonFromFace(face, poly)) {
        return INVALID_ENTITY;
    }

    auto bodyHandle = m_Backend.CreateExtrudedBody(poly, height);
    auto bodyEntity = m_Registry.CreateEntity();
    BodyComponent component;
    component.handle = bodyHandle;
    component.sourceFace = face;
    m_Registry.AddComponent(bodyEntity, component);

    m_Registry.AddComponent<NameComponent>(bodyEntity, {"ExtrudedBody"});

    // TODO: relate face ↔ body (History tracking / feature tree)
    // registry.AddComponent<ParentComponent>(bodyEntity, {m_Face});

    // TODO: keep track of last generated shape
    // m_CreatedBody = bodyEntity;
    return bodyEntity;
}

// ---------- EXPORT ----------

bool GeometrySystem::ExportSTEP(Entity body, const std::string& path) const {
    auto* bodyComp = m_Registry.GetComponent<BodyComponent>(body);
    if (!bodyComp) return false;

    return m_Backend.ExportShapeToSTEP(bodyComp->handle, path);
}

bool GeometrySystem::ExportSTL(Entity body, const std::string& path, double deflection) const {
    auto* bodyComp = m_Registry.GetComponent<BodyComponent>(body);
    if (!bodyComp) return false;

    return m_Backend.ExportShapeToSTL(bodyComp->handle, path, deflection);
}

// ---------- INTERNAL ----------

bool GeometrySystem::BuildPolygonFromFace(Entity face, Polygon& outPoly) const {
    auto* faceComp = m_Registry.GetComponent<FaceComponent>(face);
    if (!faceComp) return false;

    // For each line: get endpoints & read PositionComponents
    for (Entity edge : faceComp->edges) {
        auto* line = m_Registry.GetComponent<LineComponent>(edge);
        if (!line) return false;

        auto* p0 = m_Registry.GetComponent<PositionComponent>(line->p0);
        auto* p1 = m_Registry.GetComponent<PositionComponent>(line->p1);
        if (!p0 || !p1) return false;

        outPoly.vertices.push_back(p0->position);
    }
    return true;
}

}  // namespace nc
