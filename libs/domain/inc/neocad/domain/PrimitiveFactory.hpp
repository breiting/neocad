#pragma once

#include <glm/vec3.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/domain/Types.hpp>
#include <string>

namespace nc::domain {

/// Collection of helper functions that create ECS entities
/// for common geometric primitives (meshes & sketch geometry).
struct PrimitiveFactory {
    /// Creates a unit cube centered at origin with edge length 1.0.
    /// Adds a MeshComponent + NameComponent to the returned entity.
    static Entity MakeUnitCube(Registry& ecs, const std::string& name = "UnitCube");

    /// Creates a box with size (sx, sy, sz) centered at origin.
    static Entity MakeBox(Registry& ecs, const glm::vec3& size, const std::string& name = "Box");
};

}  // namespace nc::domain
