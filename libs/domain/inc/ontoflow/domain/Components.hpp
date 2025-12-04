#pragma once

#include <cstdint>
#include <ontoflow/domain/NodeData.hpp>
#include <ontoflow/domain/Types.hpp>
#include <string>
#include <vector>

namespace of::domain {

struct NodeComponent final {
    std::string operationID;  // Maps to f_i (Function) e.g., "MATH_SIN", "GEO_BOX"

    std::vector<Pin> inputs;
    std::vector<Pin> outputs;

    bool isDirty = true;

    // UI Metadata
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
    std::uint32_t version = 0;
};

}  // namespace of::domain
