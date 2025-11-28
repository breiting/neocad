#include <neocad/command/InsertCylinderCommand.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::cmd {

InsertCylinderCommand::InsertCylinderCommand(double radius, double height) : m_Radius(radius), m_Height(height) {
}

void InsertCylinderCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;  // Unused

    // Create Expressions
    domain::EntityID rId = registry.CreateEntity();
    domain::EntityID hId = registry.CreateEntity();

    m_ExpressionIds.push_back(rId);
    m_ExpressionIds.push_back(hId);

    domain::ExpressionComponent rExpr;
    rExpr.sourceType = domain::ExpressionComponent::SourceType::STATIC_VALUE;
    rExpr.sourceData = m_Radius;
    rExpr.evaluatedValue = m_Radius;
    rExpr.version = 1;
    registry.AddComponent(rId, rExpr);

    domain::ExpressionComponent hExpr;
    hExpr.sourceType = domain::ExpressionComponent::SourceType::STATIC_VALUE;
    hExpr.sourceData = m_Height;
    hExpr.evaluatedValue = m_Height;
    hExpr.version = 1;
    registry.AddComponent(hId, hExpr);

    // Create Cylinder Entity
    m_CylinderEntityId = registry.CreateEntity();
    registry.AddComponent(m_CylinderEntityId, domain::CylinderComponent{rId, hId});
    registry.AddComponent(m_CylinderEntityId, domain::BodyComponent{});  // Empty handle
    registry.AddComponent(m_CylinderEntityId, domain::NameComponent{"Cylinder"});
}

void InsertCylinderCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    if (m_CylinderEntityId != domain::INVALID_ENTITY) {
        registry.RemoveComponent<domain::CylinderComponent>(m_CylinderEntityId);
        registry.RemoveComponent<domain::BodyComponent>(m_CylinderEntityId);
        registry.RemoveComponent<domain::NameComponent>(m_CylinderEntityId);
        m_CylinderEntityId = domain::INVALID_ENTITY;
    }

    for (auto id : m_ExpressionIds) {
        registry.RemoveComponent<domain::ExpressionComponent>(id);
    }
    m_ExpressionIds.clear();
}

}  // namespace nc::cmd
