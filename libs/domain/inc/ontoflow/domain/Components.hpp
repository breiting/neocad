#pragma once

#include <string>
#include <vector>
#include <ontoflow/domain/NodeData.hpp>
#include <ontoflow/domain/Types.hpp>
#include <cstdint>

namespace of::domain {

struct NodeComponent {
    std::string operationID; // E.g., "MATH_SIN", "GEO_BOX"
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    bool isDirty = true;

    // Meta
    float uiX = 0.0f;
    float uiY = 0.0f;
};

struct NameComponent {
    std::string name;
};

/// Opaque handle to a backend shape (OCCT or other).
using BackendShapeHandle = std::uint64_t;

struct BodyComponent {

    BackendShapeHandle handle{0};

};



struct MeshComponent {

    Mesh mesh;

};



} // namespace of::domain
