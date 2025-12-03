#include <ontoflow/command/UpdateParameterCommand.hpp>
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/core/Logger.hpp>

namespace of::cmd {

UpdateParameterCommand::UpdateParameterCommand(domain::EntityID targetEntityId, double newValue)
    : m_TargetEntityId(targetEntityId), m_NewValue(newValue) {}

void UpdateParameterCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    // Try to update GlobalParameterComponent
    auto* globalParam = registry.GetComponent<domain::GlobalParameterComponent>(m_TargetEntityId);
    if (globalParam) {
        m_OldValue = globalParam->value;
        m_OldVersion = globalParam->version;
        globalParam->value = m_NewValue;
        globalParam->version++;
        return;
    }

    // Try to update static ExpressionComponent
    auto* expr = registry.GetComponent<domain::ExpressionComponent>(m_TargetEntityId);
    if (expr && expr->sourceType == domain::ExpressionComponent::SourceType::STATIC_VALUE) {
        if (std::holds_alternative<double>(expr->sourceData)) {
            m_OldValue = std::get<double>(expr->sourceData);
            m_OldVersion = expr->version;
            std::get<double>(expr->sourceData) = m_NewValue;
            expr->evaluatedValue = m_NewValue; // Update evaluated value immediately
            expr->version++;
            return;
        }
    }

    LOG(Error) << "UpdateParameterCommand: Target entity " << m_TargetEntityId << " is not a global parameter or a static expression.";
}

void UpdateParameterCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    // Try to undo GlobalParameterComponent
    auto* globalParam = registry.GetComponent<domain::GlobalParameterComponent>(m_TargetEntityId);
    if (globalParam) {
        globalParam->value = m_OldValue;
        globalParam->version = m_OldVersion;
        return;
    }

    // Try to undo static ExpressionComponent
    auto* expr = registry.GetComponent<domain::ExpressionComponent>(m_TargetEntityId);
    if (expr && expr->sourceType == domain::ExpressionComponent::SourceType::STATIC_VALUE) {
        if (std::holds_alternative<double>(expr->sourceData)) {
            std::get<double>(expr->sourceData) = m_OldValue;
            expr->evaluatedValue = m_OldValue;
            expr->version = m_OldVersion;
            return;
        }
    }
}

} // namespace of::cmd
