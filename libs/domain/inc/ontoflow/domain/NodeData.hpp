#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <variant>
#include <vector>
#include <ontoflow/domain/Types.hpp>

namespace of::domain {

enum class PinType { FLOW, FLOAT, INT, BOOL, VEC3, GEOMETRY, ANY };

struct GeometryHandle {
    EntityID id{INVALID_ENTITY_ID};
    
    bool IsValid() const { return id != INVALID_ENTITY_ID; }
};

using PinValue = std::variant<double, int, bool, glm::vec3, GeometryHandle>;

struct Connection {
    EntityID targetNodeID = INVALID_ENTITY_ID;
    size_t targetPinIdx = 0;
};

struct Pin {
    std::string name;
    PinType type = PinType::ANY;
    PinValue value;
    
    // Topology
    EntityID connectedNodeID = INVALID_ENTITY_ID;
    size_t connectedPinIdx = 0;
};

} // namespace of::domain
