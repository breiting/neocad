#include <algorithm>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/FeatureEvaluationSystem.hpp>

namespace nc::domain {

FeatureEvaluationSystem::FeatureEvaluationSystem(Registry& registry, IGeometryBackend& backend)
    : m_registry(registry), m_backend(backend) {
}

void FeatureEvaluationSystem::EvaluateFeatures() {
    auto boxes = m_registry.GetEntitiesWith<BoxComponent>();

    for (auto entity : boxes) {
        if (m_registry.HasComponent<BodyComponent>(entity)) {
            EvaluateBoxFeature(entity);
        }
    }

    auto cylinders = m_registry.GetEntitiesWith<CylinderComponent>();

    for (auto entity : cylinders) {
        if (m_registry.HasComponent<BodyComponent>(entity)) {
            EvaluateCylinderFeature(entity);
        }
    }
}

void FeatureEvaluationSystem::EvaluateBoxFeature(EntityID boxEntityId) {
    auto* box = m_registry.GetComponent<BoxComponent>(boxEntityId);
    auto* body = m_registry.GetComponent<BodyComponent>(boxEntityId);

    if (!box || !body)
        return;

    uint64_t maxVersion =
        GetHighestExpressionVersion({box->widthExpressionId, box->lengthExpressionId, box->heightExpressionId});

    if (body->lastRebuildVersion < maxVersion) {
        auto* wExpr = m_registry.GetComponent<ExpressionComponent>(box->widthExpressionId);
        auto* lExpr = m_registry.GetComponent<ExpressionComponent>(box->lengthExpressionId);
        auto* hExpr = m_registry.GetComponent<ExpressionComponent>(box->heightExpressionId);

        if (wExpr && lExpr && hExpr) {
            body->handle = m_backend.CreateBox(wExpr->evaluatedValue, lExpr->evaluatedValue, hExpr->evaluatedValue);
            body->lastRebuildVersion = maxVersion;
        }
    }
}

void FeatureEvaluationSystem::EvaluateCylinderFeature(EntityID cylinderEntityId) {
    auto* cyl = m_registry.GetComponent<CylinderComponent>(cylinderEntityId);
    auto* body = m_registry.GetComponent<BodyComponent>(cylinderEntityId);

    if (!cyl || !body)
        return;

    uint64_t maxVersion = GetHighestExpressionVersion({cyl->radiusExpressionId, cyl->heightExpressionId});

    if (body->lastRebuildVersion < maxVersion) {
        auto* rExpr = m_registry.GetComponent<ExpressionComponent>(cyl->radiusExpressionId);
        auto* hExpr = m_registry.GetComponent<ExpressionComponent>(cyl->heightExpressionId);

        if (rExpr && hExpr) {
            body->handle = m_backend.CreateCylinder(rExpr->evaluatedValue, hExpr->evaluatedValue);
            body->lastRebuildVersion = maxVersion;
        }
    }
}

uint64_t FeatureEvaluationSystem::GetHighestExpressionVersion(std::initializer_list<EntityID> expressionIds) {
    uint64_t maxVer = 0;
    for (auto id : expressionIds) {
        auto* expr = m_registry.GetComponent<ExpressionComponent>(id);
        if (expr) {
            maxVer = std::max(maxVer, expr->version);
        }
    }
    return maxVer;
}

}  // namespace nc::domain
