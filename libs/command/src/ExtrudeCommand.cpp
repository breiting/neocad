#include <neocad/command/ExtrudeCommand.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Entity.hpp>

using namespace nc::domain;

namespace nc::cmd {

ExtrudeCommand::ExtrudeCommand(Entity face, double height) : m_Face(face), m_Height(height) {
}

Entity ExtrudeCommand::GetFace() const {
    return m_Face;
}
double ExtrudeCommand::GetHeight() const {
    return m_Height;
}

void ExtrudeCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geom) {
    m_ResultEntity = geom.ExtrudeFace(m_Face, m_Height);
}

void ExtrudeCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geom) {
    if (m_ResultEntity != domain::INVALID_ENTITY) {
        registry.RemoveComponent<domain::BodyComponent>(m_ResultEntity);
        registry.RemoveComponent<domain::NameComponent>(m_ResultEntity);
        // Note: We don't destroy the entity ID itself, just the data.
    }
}

}  // namespace nc::cmd
