#include <neocad/command/ConnectExpressionCommand.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::cmd {

ConnectExpressionCommand::ConnectExpressionCommand(domain::EntityID targetExpressionEntityId, domain::EntityID sourceParameterEntityId)
    : m_TargetExpressionEntityId(targetExpressionEntityId), m_SourceParameterEntityId(sourceParameterEntityId) {}

void ConnectExpressionCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    auto* expr = registry.GetComponent<domain::ExpressionComponent>(m_TargetExpressionEntityId);
    if (!expr) return;

    // Save original state for Undo
    m_OriginalSourceType = expr->sourceType;
    m_OriginalSourceData = expr->sourceData;
    m_OriginalVersion = expr->version;

    // Apply new state
    expr->sourceType = domain::ExpressionComponent::SourceType::ENTITY_REFERENCE;
    expr->sourceData = m_SourceParameterEntityId;
    expr->version++; // Mark as modified
}

void ConnectExpressionCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    auto* expr = registry.GetComponent<domain::ExpressionComponent>(m_TargetExpressionEntityId);
    if (!expr) return;

    // Restore original state
    expr->sourceType = m_OriginalSourceType;
    expr->sourceData = m_OriginalSourceData;
    expr->version = m_OriginalVersion;
}

} // namespace nc::cmd
