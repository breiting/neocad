#include <neocad/domain/Components.hpp>
#include <neocad/domain/ExpressionSystem.hpp>

namespace nc::domain {

ExpressionSystem::ExpressionSystem(Registry& registry) : m_registry(registry) {
}

void ExpressionSystem::UpdateExpressions() {
    auto entities = m_registry.GetEntitiesWith<ExpressionComponent>();
    for (auto entity : entities) {
        EvaluateExpression(entity);
    }
}

void ExpressionSystem::EvaluateExpression(EntityID expressionId) {
    auto* expr = m_registry.GetComponent<ExpressionComponent>(expressionId);
    if (!expr)
        return;

    if (expr->sourceType == ExpressionComponent::SourceType::STATIC_VALUE) {
        if (std::holds_alternative<double>(expr->sourceData)) {
            expr->evaluatedValue = std::get<double>(expr->sourceData);
        }
    } else if (expr->sourceType == ExpressionComponent::SourceType::ENTITY_REFERENCE) {
        if (std::holds_alternative<EntityID>(expr->sourceData)) {
            EntityID targetId = std::get<EntityID>(expr->sourceData);
            auto* globalParam = m_registry.GetComponent<GlobalParameterComponent>(targetId);
            if (globalParam) {
                if (globalParam->version > expr->version) {
                    expr->evaluatedValue = globalParam->value;
                    expr->version = globalParam->version;
                }
            }
        }
    }
}

}  // namespace nc::domain
