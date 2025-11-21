#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace nc {

using vec3 = glm::dvec3;  // double precision for geometry
using vec2 = glm::dvec2;

/// Simple polygon in 3D space (used for face profiles).
struct Polygon {
    std::vector<vec3> vertices;  ///< assumed ordered and planar
};

}  // namespace nc
