#include <algorithm>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::domain {

Entity Registry::CreateEntity() {
    return m_NextId++;
}

// ---------- NAME ----------

template <>
void Registry::AddComponent<NameComponent>(Entity e, const NameComponent& tag) {
    m_Names[e] = tag;
}

template <>
NameComponent* Registry::GetComponent<NameComponent>(Entity e) {
    auto it = m_Names.find(e);
    return (it != m_Names.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<NameComponent>(Entity e) const {
    return m_Names.count(e) > 0;
}

// ---------- POSITION ----------

template <>
void Registry::AddComponent<PositionComponent>(Entity e, const PositionComponent& comp) {
    m_Positions[e] = comp;
}

template <>
PositionComponent* Registry::GetComponent<PositionComponent>(Entity e) {
    auto it = m_Positions.find(e);
    return (it != m_Positions.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<PositionComponent>(Entity e) const {
    return m_Positions.count(e) > 0;
}

// ---------- CIRCLES ----------

template <>
void Registry::AddComponent<RadiusComponent>(Entity e, const RadiusComponent& comp) {
    m_Circles[e] = comp;
}

template <>
RadiusComponent* Registry::GetComponent<RadiusComponent>(Entity e) {
    auto it = m_Circles.find(e);
    return (it != m_Circles.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<RadiusComponent>(Entity e) const {
    return m_Circles.count(e) > 0;
}

// ---------- LINE ----------

template <>
void Registry::AddComponent<EdgeComponent>(Entity e, const EdgeComponent& comp) {
    m_Lines[e] = comp;
}

template <>
EdgeComponent* Registry::GetComponent<EdgeComponent>(Entity e) {
    auto it = m_Lines.find(e);
    return (it != m_Lines.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<EdgeComponent>(Entity e) const {
    return m_Lines.count(e) > 0;
}

// ---------- FACE ----------
template <>
void Registry::AddComponent<FaceComponent>(Entity e, const FaceComponent& comp) {
    m_Faces[e] = comp;
}

template <>
FaceComponent* Registry::GetComponent<FaceComponent>(Entity e) {
    auto it = m_Faces.find(e);
    return (it != m_Faces.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<FaceComponent>(Entity e) const {
    return m_Faces.count(e) > 0;
}

// ---------- MESH ----------
template <>
void Registry::AddComponent<MeshComponent>(Entity e, const MeshComponent& comp) {
    m_Meshes[e] = comp;
}

template <>
MeshComponent* Registry::GetComponent<MeshComponent>(Entity e) {
    auto it = m_Meshes.find(e);
    return (it != m_Meshes.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<MeshComponent>(Entity e) const {
    return m_Meshes.count(e) > 0;
}

// ---------- SKETCHPLANE ----------
template <>
void Registry::AddComponent<SketchPlaneComponent>(Entity e, const SketchPlaneComponent& comp) {
    m_SketchPlanes[e] = comp;
}

template <>

SketchPlaneComponent* Registry::GetComponent<SketchPlaneComponent>(Entity e) {
    auto it = m_SketchPlanes.find(e);
    return (it != m_SketchPlanes.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<SketchPlaneComponent>(Entity e) const {
    return m_SketchPlanes.count(e) > 0;
}

// ---------- BODY ----------
template <>
void Registry::AddComponent<BodyComponent>(Entity e, const BodyComponent& comp) {
    m_Bodies[e] = comp;
}

template <>
BodyComponent* Registry::GetComponent<BodyComponent>(Entity e) {
    auto it = m_Bodies.find(e);
    return (it != m_Bodies.end()) ? &it->second : nullptr;
}

template <>
bool Registry::HasComponent<BodyComponent>(Entity e) const {
    return m_Bodies.count(e) > 0;
}

// ---------- METHODS ----------
//
std::vector<Entity> Registry::Entities() const {
    std::vector<Entity> result;

    auto collect = [&](const auto& map) {
        for (const auto& [e, _] : map)
            result.push_back(e);
    };

    collect(m_Names);
    collect(m_Positions);
    collect(m_Lines);
    collect(m_Faces);
    collect(m_Bodies);
    collect(m_Meshes);
    collect(m_SketchPlanes);

    // remove duplicate
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

}  // namespace nc::domain
