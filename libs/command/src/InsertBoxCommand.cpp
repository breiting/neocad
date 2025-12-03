#include <ontoflow/command/InsertBoxCommand.hpp>
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/Registry.hpp>

namespace of::cmd {

InsertBoxCommand::InsertBoxCommand(double width, double length, double height, domain::vec2 uiPosition)
    : m_Width(width), m_Length(length), m_Height(height), m_UIPosition(uiPosition) {}

void InsertBoxCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;  // Unused

    // Create Expressions
    domain::EntityID wId = registry.CreateEntity();
    domain::EntityID lId = registry.CreateEntity();
    domain::EntityID hId = registry.CreateEntity();

    m_ExpressionIds.push_back(wId);
    m_ExpressionIds.push_back(lId);
    m_ExpressionIds.push_back(hId);

    domain::ExpressionComponent wExpr;
    wExpr.sourceType = domain::ExpressionComponent::SourceType::STATIC_VALUE;
    wExpr.sourceData = m_Width;
    wExpr.evaluatedValue = m_Width;  // Pre-evaluate for simplicity
    wExpr.version = 1;               // Initial version
    registry.AddComponent(wId, wExpr);

    domain::ExpressionComponent lExpr;
    lExpr.sourceType = domain::ExpressionComponent::SourceType::STATIC_VALUE;
    lExpr.sourceData = m_Length;
    lExpr.evaluatedValue = m_Length;
    lExpr.version = 1;
    registry.AddComponent(lId, lExpr);

    domain::ExpressionComponent hExpr;
    hExpr.sourceType = domain::ExpressionComponent::SourceType::STATIC_VALUE;
    hExpr.sourceData = m_Height;
    hExpr.evaluatedValue = m_Height;
    hExpr.version = 1;
    registry.AddComponent(hId, hExpr);

    // Create Box Entity
    m_BoxEntityId = registry.CreateEntity();
    registry.AddComponent(m_BoxEntityId, domain::BoxComponent{wId, lId, hId});
    registry.AddComponent(m_BoxEntityId, domain::BodyComponent{});  // Empty handle, will be filled by System
    registry.AddComponent(m_BoxEntityId, domain::NameComponent{"Box"});
    registry.AddComponent(m_BoxEntityId, domain::UINodeComponent{static_cast<float>(m_UIPosition.x), static_cast<float>(m_UIPosition.y)});
}

void InsertBoxCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    if (m_BoxEntityId != domain::INVALID_ENTITY) {
        registry.RemoveComponent<domain::BoxComponent>(m_BoxEntityId);
        registry.RemoveComponent<domain::BodyComponent>(m_BoxEntityId);
        registry.RemoveComponent<domain::NameComponent>(m_BoxEntityId);
        registry.RemoveComponent<domain::UINodeComponent>(m_BoxEntityId);
        // TODO: Actually remove the entity itself or mark it dead.
        // Current registry has no DestroyEntity. Removing components effectively hides it from systems.
        m_BoxEntityId = domain::INVALID_ENTITY;
    }

    for (auto id : m_ExpressionIds) {
        registry.RemoveComponent<domain::ExpressionComponent>(id);
    }
    m_ExpressionIds.clear();
}

}  // namespace of::cmd
