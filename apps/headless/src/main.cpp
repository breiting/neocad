#include <neocad/command/ExtrudeCommand.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/occt/OCCTBackend.hpp>

using namespace nc;
using namespace nc::domain;
using namespace nc::occt;
using namespace nc::cmd;

int main() {
    LOG(Info) << "================================";
    LOG(Info) << "neoCAD headless testing";
    LOG(Info) << "================================";

    Registry registry;
    OCCTBackend backend;
    GeometrySystem geom(registry, backend);

    // Build a simple square face in the XY-plane
    Entity p1 = registry.CreateEntity();
    Entity p2 = registry.CreateEntity();
    Entity p3 = registry.CreateEntity();
    Entity p4 = registry.CreateEntity();

    registry.AddComponent<PositionComponent>(p1, {{0.0, 0.0, 0.0}});
    registry.AddComponent<PositionComponent>(p2, {{10.0, 0.0, 0.0}});
    registry.AddComponent<PositionComponent>(p3, {{10.0, 10.0, 0.0}});
    registry.AddComponent<PositionComponent>(p4, {{0.0, 10.0, 0.0}});

    Entity l1 = registry.CreateEntity();
    Entity l2 = registry.CreateEntity();
    Entity l3 = registry.CreateEntity();
    Entity l4 = registry.CreateEntity();

    registry.AddComponent<EdgeComponent>(l1, {p1, p2});
    registry.AddComponent<EdgeComponent>(l2, {p2, p3});
    registry.AddComponent<EdgeComponent>(l3, {p3, p4});
    registry.AddComponent<EdgeComponent>(l4, {p4, p1});

    Entity f = registry.CreateEntity();
    FaceComponent face;
    face.edges = {l1, l2, l3, l4};
    registry.AddComponent<FaceComponent>(f, face);

    // Use command pattern to extrude
    ExtrudeCommand cmd(f, 20.0);
    cmd.Execute(registry, geom);

    // Find the created body
    Entity bodyEntity = INVALID_ENTITY;
    auto res = HasComponentQuery<BodyComponent>().Execute(registry);
    if (res.empty()) {
        LOG(Error) << "Cannot find created body.";
        return 1;
    }
    bodyEntity = res.front();
    LOG(Info) << "Found body entity: " << bodyEntity;

    // Export
    if (!geom.ExportSTEP(bodyEntity, "test_body.step")) {
        LOG(Error) << "Failed to export STEP.\n";
    } else {
        LOG(Info) << "Exported STEP: test_body.step\n";
    }

    if (!geom.ExportSTL(bodyEntity, "test_body.stl", 0.5)) {
        LOG(Error) << "Failed to export STL.\n";
    } else {
        LOG(Info) << "Exported STL: test_body.stl\n";
    }

    return 0;
}
