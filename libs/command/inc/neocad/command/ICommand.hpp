#pragma once

namespace nc::cmd {

class Registry;
class GeometrySystem;

/**
 * \brief Command interface, which executed on Registry and GeometrySystem
 */
class ICommand {
   public:
    virtual ~ICommand() = default;
    virtual void Execute(Registry& registry, GeometrySystem& geom) = 0;
};

}  // namespace nc
