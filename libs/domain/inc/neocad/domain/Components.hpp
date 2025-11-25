#pragma once
#include <glm/vec3.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/Types.hpp>
#include <string>
#include <vector>

namespace nc::domain {

/// Name/tag
struct NameComponent {
    std::string name;
};

/// 3D position
struct PositionComponent {
    vec3 position;
};

/// Line between two point entities
struct LineComponent {
    Entity p0{INVALID_ENTITY};
    Entity p1{INVALID_ENTITY};
};

/// Planar face defined by a sequence of line entities forming a closed loop.
struct FaceComponent {
    std::vector<Entity> edges;  ///< entities referring to LineComponent
};

/// Opaque handle to a backend shape (OCCT or other).
using BackendShapeHandle = std::uint64_t;

/// 3D body (solid) referencing a backend shape.
struct BodyComponent {
    BackendShapeHandle handle{0};
    Entity sourceFace{INVALID_ENTITY};  ///< originating face entity (optional)
};

/// Component for defining a circle
struct RadiusComponent {
    double radius;
};

/// 3D Mesh component for rendering
struct MeshComponent {
    TriMesh mesh;
};

/// SketchPlane component which acts as a proxy for 2D sketching
struct SketchPlaneComponent {
    vec3 origin{0, 0, 0};
    vec3 normal{0, 0, 1};  // Z = Up
    vec3 xdir{1, 0, 0};
    vec3 ydir{0, 1, 0};
    bool active = false;
};

}  // namespace nc
