#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Types.hpp>

namespace nc::cmd {

/// Command to update the value of a global parameter or a static expression.
class UpdateParameterCommand : public ICommand {
public:
    /// \brief Constructs an UpdateParameterCommand.
    /// \param targetEntityId The EntityID of the GlobalParameterComponent or ExpressionComponent to update.
    /// \param newValue The new value to set.
    UpdateParameterCommand(domain::EntityID targetEntityId, double newValue);

    void Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    void Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    
    std::string GetName() const override { return "Update Parameter"; }

private:
    domain::EntityID m_TargetEntityId;
    double m_NewValue;
    double m_OldValue; // Stored for Undo
    uint64_t m_OldVersion; // Stored for Undo
};

} // namespace nc::cmd
