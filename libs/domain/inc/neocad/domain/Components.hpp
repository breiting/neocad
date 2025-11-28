#pragma once
#include <glm/vec3.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/Types.hpp>
#include <string>
#include <variant>
#include <vector>

namespace nc::domain {

/// Name/tag
struct NameComponent {
    std::string name;
};

struct GlobalParameterComponent final {
    std::string name;
    double value = 0.0;
    std::uint64_t version = 0;
};

struct ExpressionComponent final {
    double evaluatedValue = 0.0;
    std::uint64_t version = 0;

    enum class SourceType {
        STATIC_VALUE,
        ENTITY_REFERENCE
    };
    SourceType sourceType = SourceType::STATIC_VALUE;

    std::variant<double, EntityID> sourceData;
};

/// Component defining a parametric box.
struct BoxComponent final {
    EntityID widthExpressionId;
    EntityID lengthExpressionId;
    EntityID heightExpressionId;
};

/// Component defining a parametric cylinder.
struct CylinderComponent final {
    EntityID radiusExpressionId;
    EntityID heightExpressionId;
};

/// Stores the position of the node in the 2D graph editor window (for persistence).
struct UINodeComponent final {
    float positionX = 0.0f;
    float positionY = 0.0f;
    bool isCollapsed = false;
};

/// 3D position
struct PositionComponent {
    vec3 position;
};

/// Connection (edge) between two point entities
struct EdgeComponent {
    Entity p0{INVALID_ENTITY};
    Entity p1{INVALID_ENTITY};
};

/// Planar face defined by a sequence of edge entities forming a closed loop.
struct FaceComponent {
    std::vector<Entity> vertices;  ///< entities referring to PositionComponent
    std::vector<Entity> edges;     ///< entities referring to EdgeComponent
};

/// Opaque handle to a backend shape (OCCT or other).
using BackendShapeHandle = std::uint64_t;

/// 3D body (solid) referencing a backend shape.
struct BodyComponent {
    BackendShapeHandle handle{0};
    std::uint64_t lastRebuildVersion = 0;
};

/// Component for defining a circle
struct RadiusComponent {
    double radius = 1.0;
};

/// 3D Mesh component for rendering
struct MeshComponent {
    Mesh mesh;
};

/// SketchPlane component which acts as a proxy for 2D sketching
struct SketchPlaneComponent {
    vec3 origin{0, 0, 0};
    vec3 normal{0, 0, 1};  // Z = Up
    vec3 xdir{1, 0, 0};
    vec3 ydir{0, 1, 0};
    bool active = false;
};

}  // namespace nc::domain
