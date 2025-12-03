#include <gtest/gtest.h>

#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/ExpressionSystem.hpp>
#include <ontoflow/domain/FeatureEvaluationSystem.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/occt/OCCTBackend.hpp>

using namespace of::domain;

TEST(TestMRA, BoxParametricLoop) {
    Registry registry;
    of::occt::OCCTBackend backend;
    ExpressionSystem expressionSystem(registry);
    FeatureEvaluationSystem featureSystem(registry, backend);

    EntityID widthId = registry.CreateEntity();
    registry.AddComponent(widthId, GlobalParameterComponent{"WIDTH", 10.0, 1});

    EntityID heightId = registry.CreateEntity();
    registry.AddComponent(heightId, GlobalParameterComponent{"HEIGHT", 20.0, 1});

    EntityID boxId = registry.CreateEntity();
    // Body component initially with invalid handle and version 0
    registry.AddComponent(boxId, BodyComponent{});

    // ExpressionComponents for Box (W, L, H)
    EntityID wExprId = registry.CreateEntity();
    ExpressionComponent wExpr;
    wExpr.sourceType = ExpressionComponent::SourceType::ENTITY_REFERENCE;
    wExpr.sourceData = widthId;
    registry.AddComponent(wExprId, wExpr);

    EntityID hExprId = registry.CreateEntity();
    ExpressionComponent hExpr;
    hExpr.sourceType = ExpressionComponent::SourceType::ENTITY_REFERENCE;
    hExpr.sourceData = heightId;
    registry.AddComponent(hExprId, hExpr);

    // L: STATIC = 15.0
    EntityID lExprId = registry.CreateEntity();
    ExpressionComponent lExpr;
    lExpr.sourceType = ExpressionComponent::SourceType::STATIC_VALUE;
    lExpr.sourceData = 15.0;
    // Static values are considered version 0 or handled effectively.
    registry.AddComponent(lExprId, lExpr);

    // Attach BoxComponent
    registry.AddComponent(boxId, BoxComponent{wExprId, lExprId, hExprId});

    expressionSystem.UpdateExpressions();

    // Check if expressions evaluated
    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(wExprId)->evaluatedValue, 10.0);
    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(hExprId)->evaluatedValue, 20.0);
    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(lExprId)->evaluatedValue, 15.0);

    featureSystem.EvaluateFeatures();

    // Check BodyComponent
    auto* body = registry.GetComponent<BodyComponent>(boxId);
    ASSERT_NE(body, nullptr);
    EXPECT_NE(body->handle, 0);
    // Expected version depends on how highest version works.
    // Width=1, Height=1, Static=0 -> Max=1.
    EXPECT_GE(body->lastRebuildVersion, 1);
    uint64_t firstVersion = body->lastRebuildVersion;

    // Change global parameter
    auto* widthParam = registry.GetComponent<GlobalParameterComponent>(widthId);
    widthParam->value = 30.0;
    widthParam->version++;  // Increment version to 2

    expressionSystem.UpdateExpressions();

    // Verify expression updated
    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(wExprId)->evaluatedValue, 30.0);
    EXPECT_EQ(registry.GetComponent<ExpressionComponent>(wExprId)->version, widthParam->version);

    featureSystem.EvaluateFeatures();

    // Check if Box has been updated
    EXPECT_GT(body->lastRebuildVersion, firstVersion);
    EXPECT_EQ(body->lastRebuildVersion, widthParam->version);
    EXPECT_NE(body->handle, 0);
}
