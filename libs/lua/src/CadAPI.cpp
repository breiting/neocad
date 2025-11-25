#include <neocad/domain/Components.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/lua/CadAPI.hpp>

using namespace nc::domain;

namespace nc::lua {

CadAPI::CadAPI(Registry& reg, GeometrySystem& geom) : m_Registry(reg), m_GeometrySystem(geom) {
}

Entity CadAPI::CreatePoint(double x, double y, double z) {
    Entity e = m_Registry.CreateEntity();
    m_Registry.AddComponent<PositionComponent>(e, {glm::vec3(x, y, z)});
    return e;
}

Entity CadAPI::CreateLine(Entity p1, Entity p2) {
    Entity e = m_Registry.CreateEntity();
    m_Registry.AddComponent<LineComponent>(e, {p1, p2});
    return e;
}

Entity CadAPI::CreateFace(const std::vector<Entity>& pts) {
    Entity e = m_Registry.CreateEntity();
    m_Registry.AddComponent<FaceComponent>(e, {pts});
    return e;
}

Entity CadAPI::ExtrudeFace(Entity face, double height) {
    return m_GeometrySystem.ExtrudeFace(face, height);
}

bool CadAPI::ExportSTEP(Entity body, const std::string& path) {
    return m_GeometrySystem.ExportSTEP(body, path);
}

bool CadAPI::ExportSTL(Entity body, const std::string& path, double deflection) {
    return m_GeometrySystem.ExportSTL(body, path, deflection);
}

}  // namespace nc::lua
