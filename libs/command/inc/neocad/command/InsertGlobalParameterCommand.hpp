#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Types.hpp>

namespace nc::cmd {

/// Command to insert a global parameter into the registry.
class InsertGlobalParameterCommand : public ICommand {
public:
    InsertGlobalParameterCommand(std::string name, double initialValue, domain::vec2 uiPosition);

    void Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    void Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    
    std::string GetName() const override { return "Insert Global Parameter"; }

private:
    std::string m_Name;
    double m_InitialValue;
    domain::vec2 m_UIPosition;
    
    domain::EntityID m_EntityId{domain::INVALID_ENTITY};
};

} // namespace nc::cmd
