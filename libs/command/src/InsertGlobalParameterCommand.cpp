#include <neocad/command/InsertGlobalParameterCommand.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::cmd {

InsertGlobalParameterCommand::InsertGlobalParameterCommand(std::string name, double initialValue, domain::vec2 uiPosition)
    : m_Name(std::move(name)), m_InitialValue(initialValue), m_UIPosition(uiPosition) {}

void InsertGlobalParameterCommand::Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    m_EntityId = registry.CreateEntity();
    registry.AddComponent(m_EntityId, domain::GlobalParameterComponent{m_Name, m_InitialValue, 1});
    registry.AddComponent(m_EntityId, domain::UINodeComponent{static_cast<float>(m_UIPosition.x), static_cast<float>(m_UIPosition.y)});
}

void InsertGlobalParameterCommand::Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) {
    (void)geometrySystem;

    if (m_EntityId != domain::INVALID_ENTITY) {
        registry.RemoveComponent<domain::GlobalParameterComponent>(m_EntityId);
        registry.RemoveComponent<domain::UINodeComponent>(m_EntityId);
        m_EntityId = domain::INVALID_ENTITY;
    }
}

} // namespace nc::cmd
