#pragma once
#include <glm/vec3.hpp>
#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/domain/Types.hpp>
#include <string>

namespace of::domain {

/// Name/tag
struct NameComponent {
    std::string name;
};

/// Stores the position of the node in the 2D graph editor window (for persistence).
struct UINodeComponent final {
    float positionX = 0.0f;
    float positionY = 0.0f;
    bool isCollapsed = false;
};

/// Opaque handle to a backend shape (OCCT or other).
using BackendShapeHandle = std::uint64_t;

/// 3D body (solid) referencing a backend shape.
struct BodyComponent {
    BackendShapeHandle handle{0};
    std::uint64_t lastRebuildVersion = 0;
};

/// 3D Mesh component for rendering
struct MeshComponent {
    Mesh mesh;
};

}  // namespace of::domain
