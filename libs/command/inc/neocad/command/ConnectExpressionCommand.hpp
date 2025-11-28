#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Types.hpp>

namespace nc::cmd {

/// Command to connect an expression (input pin) to a global parameter (output pin).
class ConnectExpressionCommand : public ICommand {
public:
    /// \brief Constructs a ConnectExpressionCommand.
    /// \param targetExpressionEntityId The EntityID of the expression component to modify.
    /// \param sourceParameterEntityId The EntityID of the global parameter to reference.
    ConnectExpressionCommand(domain::EntityID targetExpressionEntityId, domain::EntityID sourceParameterEntityId);

    void Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    void Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    
    std::string GetName() const override { return "Connect Expression"; }

private:
    domain::EntityID m_TargetExpressionEntityId;
    domain::EntityID m_SourceParameterEntityId;

    // Data to revert to on Undo
    domain::ExpressionComponent::SourceType m_OriginalSourceType;
    std::variant<double, domain::EntityID> m_OriginalSourceData;
    uint64_t m_OriginalVersion;
};

} // namespace nc::cmd
