#pragma once

#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::cmd {

/**
 * \brief Command interface, which executed on Registry and GeometrySystem
 */
class ICommand {
   public:
    virtual ~ICommand() = default;
    virtual void Execute(domain::Registry& registry, domain::GeometrySystem& geom) = 0;
};

}  // namespace nc::cmd
