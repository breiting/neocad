// clang-format off
#include <glad.h>
// clang-format on
#include <neocad/rendering/BaseGeometry.hpp>

using namespace nc::domain;

namespace nc::vis {

BaseGeometry::BaseGeometry() : m_Dirty(true) {
}

void BaseGeometry::AddVertex(const Vertex& v) {
    m_Vertices.push_back(v);
    m_Dirty = true;
}

void BaseGeometry::SetVertices(const std::vector<domain::Vertex>& vertices) {
    m_Vertices = vertices;
}

size_t BaseGeometry::VertexCount() const {
    return m_Vertices.size();
}

const std::vector<Vertex>& BaseGeometry::GetVertices() const {
    return m_Vertices;
}
}  // namespace nc::vis
