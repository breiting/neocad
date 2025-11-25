#pragma once
#include <neocad/domain/Vertex.hpp>
#include <vector>

namespace nc::domain {

using vec3 = glm::dvec3;  // double precision for geometry
using vec2 = glm::dvec2;

/// Simple polygon in double precision in 3D space (used for face profiles).
struct Polygon {
    std::vector<vec3> vertices;  ///< assumed ordered and planar
};

struct TriMesh {
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    /// \brief Clears all mesh data.
    void Clear() {
        vertices.clear();
        indices.clear();
    }

    /// \brief Returns true if the mesh has no vertices.
    bool IsEmpty() const noexcept {
        return vertices.empty();
    }
};

}  // namespace nc::domain
