#pragma once

#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>
#include <string>

namespace nc::cmd {

/**
 * \brief Command interface with Undo/Redo support.
 */
class ICommand {
   public:
    virtual ~ICommand() = default;

    /// Execute the command (do the work)
    virtual void Execute(domain::Registry& registry, domain::GeometrySystem& geom) = 0;

    /// Revert the command (undo the work)
    virtual void Undo(domain::Registry& registry, domain::GeometrySystem& geom) = 0;
    
    /// Optional: Name for the UI (e.g. "Create Point")
    virtual std::string GetName() const { return "Command"; }
};

}  // namespace nc::cmd
