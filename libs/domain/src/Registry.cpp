#include <algorithm>
#include <iostream>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::domain {

std::vector<Entity> Registry::Entities() const {
    std::vector<Entity> result;

    auto collect = [&](const auto& map) {
        for (auto& [e, _] : map)
            result.push_back(e);
    };

    // WICHTIG: hier ALLE Component-Typen auflisten, die es aktuell gibt
    collect(StorageConst<NameComponent>());
    collect(StorageConst<PositionComponent>());
    collect(StorageConst<RadiusComponent>());
    collect(StorageConst<EdgeComponent>());
    collect(StorageConst<FaceComponent>());
    collect(StorageConst<MeshComponent>());
    collect(StorageConst<SketchPlaneComponent>());
    collect(StorageConst<BodyComponent>());

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

void Registry::Dump() const {
    std::cout << "\n=== REGISTRY DUMP ===\n";

    for (Entity e : Entities()) {
        std::cout << "Entity " << e << "\n";

        if (HasComponent<NameComponent>(e)) {
            auto* c = GetComponent<NameComponent>(e);
            std::cout << "  Name        = " << c->name << "\n";
        }

        if (HasComponent<PositionComponent>(e)) {
            auto* c = GetComponent<PositionComponent>(e);
            std::cout << "  Position    = (" << c->position.x << ", " << c->position.y << ", " << c->position.z
                      << ")\n";
        }

        if (HasComponent<RadiusComponent>(e)) {
            auto* c = GetComponent<RadiusComponent>(e);
            std::cout << "  Radius      = " << c->radius << "\n";
        }

        if (HasComponent<EdgeComponent>(e)) {
            auto* c = GetComponent<EdgeComponent>(e);
            std::cout << "  Edge        = p0=" << c->p0 << ", p1=" << c->p1 << "\n";
        }

        if (HasComponent<FaceComponent>(e)) {
            auto* c = GetComponent<FaceComponent>(e);
            std::cout << "  Face        = vertices=" << c->vertices.size() << ", edges=" << c->edges.size() << "\n";
        }

        if (HasComponent<MeshComponent>(e)) {
            auto* c = GetComponent<MeshComponent>(e);
            std::cout << "  Mesh        = vtx=" << c->mesh.vertices.size() << ", idx=" << c->mesh.indices.size()
                      << "\n";
        }

        if (HasComponent<SketchPlaneComponent>(e)) {
            auto* c = GetComponent<SketchPlaneComponent>(e);
            std::cout << "  SketchPlane = origin=(" << c->origin.x << ", " << c->origin.y << ", " << c->origin.z << ")"
                      << " normal=(" << c->normal.x << ", " << c->normal.y << ", " << c->normal.z << ")\n";
        }

        if (HasComponent<BodyComponent>(e)) {
            std::cout << "  Body        = [OCCT handle present]\n";
        }

        std::cout << "\n";
    }

    std::cout << "=== END REGISTRY ===\n\n";
}

}  // namespace nc::domain
