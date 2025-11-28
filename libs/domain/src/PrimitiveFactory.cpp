#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <neocad/domain/PrimitiveFactory.hpp>

namespace nc::domain {

namespace {

/// Helper: push a quad (4 vertices) into a mesh with a constant normal.
/// v0-v1-v2-v3 must be in CCW order (as seen from outside).
inline void AddQuadFace(Mesh& mesh, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
                        const glm::vec3& normal) {
    const uint32_t baseIndex = static_cast<uint32_t>(mesh.vertices.size());

    mesh.vertices.emplace_back(v0, normal);
    mesh.vertices.emplace_back(v1, normal);
    mesh.vertices.emplace_back(v2, normal);
    mesh.vertices.emplace_back(v3, normal);

    // two triangles: (0,1,2) (0,2,3)
    mesh.indices.push_back(baseIndex + 0);
    mesh.indices.push_back(baseIndex + 1);
    mesh.indices.push_back(baseIndex + 2);

    mesh.indices.push_back(baseIndex + 0);
    mesh.indices.push_back(baseIndex + 2);
    mesh.indices.push_back(baseIndex + 3);
}

}  // namespace

// ----------------------------------------------------
// Unit cube (edge length 1, centered at origin)
// ----------------------------------------------------
Entity PrimitiveFactory::MakeUnitCube(Registry& ecs, const std::string& name) {
    MeshComponent mc;

    // half extents for unit cube
    glm::vec3 he(0.5f, 0.5f, 0.5f);

    // FRONT (+Z)
    AddQuadFace(mc.mesh, {-he.x, -he.y, +he.z}, {+he.x, -he.y, +he.z}, {+he.x, +he.y, +he.z}, {-he.x, +he.y, +he.z},
                {0.0f, 0.0f, 1.0f});

    // BACK (-Z)
    AddQuadFace(mc.mesh, {-he.x, -he.y, -he.z}, {-he.x, +he.y, -he.z}, {+he.x, +he.y, -he.z}, {+he.x, -he.y, -he.z},
                {0.0f, 0.0f, -1.0f});

    // LEFT (-X)
    AddQuadFace(mc.mesh, {-he.x, -he.y, -he.z}, {-he.x, -he.y, +he.z}, {-he.x, +he.y, +he.z}, {-he.x, +he.y, -he.z},
                {-1.0f, 0.0f, 0.0f});

    // RIGHT (+X)
    AddQuadFace(mc.mesh, {+he.x, -he.y, -he.z}, {+he.x, +he.y, -he.z}, {+he.x, +he.y, +he.z}, {+he.x, -he.y, +he.z},
                {1.0f, 0.0f, 0.0f});

    // TOP (+Y)
    AddQuadFace(mc.mesh, {-he.x, +he.y, -he.z}, {-he.x, +he.y, +he.z}, {+he.x, +he.y, +he.z}, {+he.x, +he.y, -he.z},
                {0.0f, 1.0f, 0.0f});

    // BOTTOM (-Y)
    AddQuadFace(mc.mesh, {-he.x, -he.y, -he.z}, {+he.x, -he.y, -he.z}, {+he.x, -he.y, +he.z}, {-he.x, -he.y, +he.z},
                {0.0f, -1.0f, 0.0f});

    Entity e = ecs.CreateEntity();
    ecs.AddComponent<MeshComponent>(e, mc);
    ecs.AddComponent<NameComponent>(e, NameComponent{name});

    return e;
}

// ----------------------------------------------------
// Box of arbitrary size (sx, sy, sz)
// ----------------------------------------------------
Entity PrimitiveFactory::MakeBox(Registry& ecs, const glm::vec3& size, const std::string& name) {
    MeshComponent mc;

    glm::vec3 he = 0.5f * size;  // half extents

    // FRONT (+Z)
    AddQuadFace(mc.mesh, {-he.x, -he.y, +he.z}, {+he.x, -he.y, +he.z}, {+he.x, +he.y, +he.z}, {-he.x, +he.y, +he.z},
                {0.0f, 0.0f, 1.0f});

    // BACK (-Z)
    AddQuadFace(mc.mesh, {-he.x, -he.y, -he.z}, {-he.x, +he.y, -he.z}, {+he.x, +he.y, -he.z}, {+he.x, -he.y, -he.z},
                {0.0f, 0.0f, -1.0f});

    // LEFT (-X)
    AddQuadFace(mc.mesh, {-he.x, -he.y, -he.z}, {-he.x, -he.y, +he.z}, {-he.x, +he.y, +he.z}, {-he.x, +he.y, -he.z},
                {-1.0f, 0.0f, 0.0f});

    // RIGHT (+X)
    AddQuadFace(mc.mesh, {+he.x, -he.y, -he.z}, {+he.x, +he.y, -he.z}, {+he.x, +he.y, +he.z}, {+he.x, -he.y, +he.z},
                {1.0f, 0.0f, 0.0f});

    // TOP (+Y)
    AddQuadFace(mc.mesh, {-he.x, +he.y, -he.z}, {-he.x, +he.y, +he.z}, {+he.x, +he.y, +he.z}, {+he.x, +he.y, -he.z},
                {0.0f, 1.0f, 0.0f});

    // BOTTOM (-Y)
    AddQuadFace(mc.mesh, {-he.x, -he.y, -he.z}, {+he.x, -he.y, -he.z}, {+he.x, -he.y, +he.z}, {-he.x, -he.y, +he.z},
                {0.0f, -1.0f, 0.0f});

    Entity e = ecs.CreateEntity();
    ecs.AddComponent<MeshComponent>(e, mc);
    ecs.AddComponent<NameComponent>(e, NameComponent{name});

    return e;
}

}  // namespace nc::domain
