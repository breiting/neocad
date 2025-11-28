#include <gtest/gtest.h>
#include <neocad/domain/Registry.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/ExpressionSystem.hpp>
#include <neocad/domain/FeatureEvaluationSystem.hpp>
#include <neocad/occt/OCCTBackend.hpp>

using namespace nc::domain;

TEST(TestCylinder, CylinderParametricLoop) {
    Registry registry;
    nc::occt::OCCTBackend backend;
    ExpressionSystem expressionSystem(registry);
    FeatureEvaluationSystem featureSystem(registry, backend);

    // 1. Create global parameter for Radius
    EntityID radiusParamId = registry.CreateEntity();
    registry.AddComponent(radiusParamId, GlobalParameterComponent{"RADIUS", 5.0, 1});

    // 2. Create Cylinder Entity
    EntityID cylinderId = registry.CreateEntity();
    registry.AddComponent(cylinderId, BodyComponent{}); 

    // 3. Create ExpressionComponents
    // Radius: REF to RADIUS
    EntityID rExprId = registry.CreateEntity();
    ExpressionComponent rExpr;
    rExpr.sourceType = ExpressionComponent::SourceType::ENTITY_REFERENCE;
    rExpr.sourceData = radiusParamId;
    registry.AddComponent(rExprId, rExpr);

    // Height: STATIC = 10.0
    EntityID hExprId = registry.CreateEntity();
    ExpressionComponent hExpr;
    hExpr.sourceType = ExpressionComponent::SourceType::STATIC_VALUE;
    hExpr.sourceData = 10.0;
    registry.AddComponent(hExprId, hExpr);

    // Attach CylinderComponent
    registry.AddComponent(cylinderId, CylinderComponent{rExprId, hExprId});

    // 4. Evaluate Expressions
    expressionSystem.UpdateExpressions();

    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(rExprId)->evaluatedValue, 5.0);
    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(hExprId)->evaluatedValue, 10.0);

    // 5. Evaluate Features
    featureSystem.EvaluateFeatures();

    // Check BodyComponent
    auto* body = registry.GetComponent<BodyComponent>(cylinderId);
    ASSERT_NE(body, nullptr);
    EXPECT_NE(body->handle, 0);
    EXPECT_GE(body->lastRebuildVersion, 1);
    uint64_t firstVersion = body->lastRebuildVersion;

    // 6. Change RADIUS
    auto* radiusParam = registry.GetComponent<GlobalParameterComponent>(radiusParamId);
    radiusParam->value = 8.0;
    radiusParam->version++; 

    // 7. Update again
    expressionSystem.UpdateExpressions();
    
    EXPECT_DOUBLE_EQ(registry.GetComponent<ExpressionComponent>(rExprId)->evaluatedValue, 8.0);
    EXPECT_EQ(registry.GetComponent<ExpressionComponent>(rExprId)->version, radiusParam->version);

    featureSystem.EvaluateFeatures();

    // 8. Check BodyComponent updated
    EXPECT_GT(body->lastRebuildVersion, firstVersion);
    EXPECT_EQ(body->lastRebuildVersion, radiusParam->version);
    EXPECT_NE(body->handle, 0); 
}
