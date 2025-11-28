#include <gtest/gtest.h>

#include <neocad/domain/Components.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/Registry.hpp>

#include "neocad/domain/Entity.hpp"

using namespace nc::domain;

/// Simple fake backend for testing without OCCT.
class FakeBackend : public IGeometryBackend {
   public:
    BackendShapeHandle CreateExtrudedBody(const Polygon& profile, double height) override {
        lastProfile = profile;
        lastHeight = height;
        return ++lastHandle;
    }

    BackendShapeHandle CreateBox(double width, double length, double height) override {
        return ++lastHandle;
    }

    BackendShapeHandle CreateCylinder(double radius, double height) override {
        return ++lastHandle;
    }

    bool ExportShapeToSTEP(BackendShapeHandle, const std::string&) const override {
        return true;
    }

    bool ExportShapeToSTL(BackendShapeHandle, const std::string&, double) const override {
        return true;
    }

    Polygon lastProfile;
    double lastHeight{0.0};
    BackendShapeHandle lastHandle{0};
};

TEST(GeometrySystemTest, ExtrudeFaceCreatesBody) {
    Registry reg;
    FakeBackend backend;
    GeometrySystem geom(reg, backend);

    // Build a simple rectangle face: 4 points, 4 lines
    Entity p1 = reg.CreateEntity();
    Entity p2 = reg.CreateEntity();
    Entity p3 = reg.CreateEntity();
    Entity p4 = reg.CreateEntity();

    reg.AddComponent<PositionComponent>(p1, {{0.0, 0.0, 0.0}});
    reg.AddComponent<PositionComponent>(p2, {{1.0, 0.0, 0.0}});
    reg.AddComponent<PositionComponent>(p3, {{1.0, 1.0, 0.0}});
    reg.AddComponent<PositionComponent>(p4, {{0.0, 1.0, 0.0}});

    Entity l1 = reg.CreateEntity();
    Entity l2 = reg.CreateEntity();
    Entity l3 = reg.CreateEntity();
    Entity l4 = reg.CreateEntity();

    reg.AddComponent<EdgeComponent>(l1, {p1, p2});
    reg.AddComponent<EdgeComponent>(l2, {p2, p3});
    reg.AddComponent<EdgeComponent>(l3, {p3, p4});
    reg.AddComponent<EdgeComponent>(l4, {p4, p1});

    Entity f = reg.CreateEntity();
    FaceComponent face;
    face.edges = {l1, l2, l3, l4};
    reg.AddComponent<FaceComponent>(f, face);

    double height = 5.0;
    Entity body = geom.ExtrudeFace(f, height);

    ASSERT_NE(body, INVALID_ENTITY);
    const BodyComponent* b = reg.GetComponent<BodyComponent>(body);
    ASSERT_NE(b, nullptr);
    EXPECT_GT(b->handle, 0u);

    // Check backend got correct profile
    EXPECT_EQ(backend.lastProfile.vertices.size(), 4u);
    EXPECT_DOUBLE_EQ(backend.lastHeight, height);
}
