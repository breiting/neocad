#include <ontoflow/command/ExtrudeCommand.hpp>
#include <ontoflow/core/Logger.hpp>
#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/domain/Components.hpp> // Required for BodyComponent/NameComponent in Undo

using namespace of::domain;

namespace of::cmd {

ExtrudeCommand::ExtrudeCommand(Entity face, double height) : m_Face(face), m_Height(height) {
}

Entity ExtrudeCommand::GetFace() const {
    return m_Face;
}
double ExtrudeCommand::GetHeight() const {
    return m_Height;
}

void ExtrudeCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geom) {
    LOG(Info) << "Executing ExtrudeCommand: face=" << m_Face << ", height=" << m_Height;
    m_ResultEntity = geom.ExtrudeFace(m_Face, m_Height);
    if (m_ResultEntity == domain::INVALID_ENTITY) {
        LOG(Error) << "ExtrudeCommand: Failed to extrude face " << m_Face;
    } else {
        LOG(Info) << "ExtrudeCommand: Created body " << m_ResultEntity;
    }
}

void ExtrudeCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geom) {
    if (m_ResultEntity != domain::INVALID_ENTITY) {
        LOG(Info) << "Undoing ExtrudeCommand: removing body " << m_ResultEntity;
        registry.RemoveComponent<domain::BodyComponent>(m_ResultEntity);
        registry.RemoveComponent<domain::NameComponent>(m_ResultEntity);
        m_ResultEntity = domain::INVALID_ENTITY; // Mark as undone
    }
}

}  // namespace of::cmd
