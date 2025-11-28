#include <algorithm>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::domain {

std::vector<Entity> Registry::Entities() const {
    std::vector<Entity> result;

    for (const auto& kv : m_Storages) {
        // kv.second is unique_ptr<IComponentStorage>
        if (kv.second) {
            std::vector<Entity> ents = kv.second->GetEntities();
            result.insert(result.end(), ents.begin(), ents.end());
        }
    }

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

void Registry::Dump() const {
    LOG(Info) << "\n=== REGISTRY DUMP ===";

    for (Entity e : Entities()) {
        LOG(Info) << "Entity " << e;

        if (HasComponent<NameComponent>(e)) {
            auto* c = GetComponent<NameComponent>(e);
            LOG(Info) << "  Name        = " << c->name;
        }

        if (HasComponent<PositionComponent>(e)) {
            auto* c = GetComponent<PositionComponent>(e);
            LOG(Info) << "  Position    = (" << c->position.x << ", " << c->position.y << ", " << c->position.z
                      << ")";
        }

        if (HasComponent<RadiusComponent>(e)) {
            auto* c = GetComponent<RadiusComponent>(e);
            LOG(Info) << "  Radius      = " << c->radius;
        }

        if (HasComponent<EdgeComponent>(e)) {
            auto* c = GetComponent<EdgeComponent>(e);
            LOG(Info) << "  Edge        = p0=" << c->p0 << ", p1=" << c->p1;
        }

        if (HasComponent<FaceComponent>(e)) {
            auto* c = GetComponent<FaceComponent>(e);
            LOG(Info) << "  Face        = vertices=" << c->vertices.size() << ", edges=" << c->edges.size();
        }

        if (HasComponent<MeshComponent>(e)) {
            auto* c = GetComponent<MeshComponent>(e);
            LOG(Info) << "  Mesh        = vtx=" << c->mesh.vertices.size() << ", idx=" << c->mesh.indices.size();
        }

        if (HasComponent<SketchPlaneComponent>(e)) {
            auto* c = GetComponent<SketchPlaneComponent>(e);
            LOG(Info) << "  SketchPlane = origin=(" << c->origin.x << ", " << c->origin.y << ", " << c->origin.z << ")"
                      << " normal=(" << c->normal.x << ", " << c->normal.y << ", " << c->normal.z << ")";
        }

        if (HasComponent<BodyComponent>(e)) {
            LOG(Info) << "  Body        = [OCCT handle present]";
        }
    }

    LOG(Info) << "=== END REGISTRY ===\n";
}

}  // namespace nc::domain
