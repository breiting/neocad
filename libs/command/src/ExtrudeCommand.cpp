#include <neocad/command/ExtrudeCommand.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc {

ExtrudeCommand::ExtrudeCommand(Entity face, double height) : m_Face(face), m_Height(height) {
}

Entity ExtrudeCommand::GetFace() const {
    return m_Face;
}
double ExtrudeCommand::GetHeight() const {
    return m_Height;
}

void ExtrudeCommand::Execute(Registry& registry, GeometrySystem& geom) {
    Entity bodyEntity = geom.ExtrudeFace(m_Face, m_Height);

    if (bodyEntity == INVALID_ENTITY) {
        LOG(ERROR) << "Error in extruding face";
    }
}

}  // namespace nc
