#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <variant>
#include <vector>
#include <ontoflow/domain/Types.hpp>

namespace of::domain {

enum class PinType { FLOAT, INT, BOOL, VEC3, GEOMETRY, ANY };

// Wrapper to safely store Geometry References
struct GeometryHandle { EntityID id; };

// The Data flowing through the graph
using PinValue = std::variant<double, int, bool, glm::vec3, GeometryHandle, std::monostate>;

// The Edge E = (TargetNode, TargetPin)
struct Connection {
    EntityID targetNodeID = INVALID_ENTITY_ID; // 0 is invalid
    size_t targetPinIdx = 0;
};

// The Pin (Input or Output slot)
struct Pin {
    std::string name;
    PinType type = PinType::FLOAT;
    PinValue value;        // Static data or Cache
    Connection connection; // Topology (Input only)
};

} // namespace of::domain