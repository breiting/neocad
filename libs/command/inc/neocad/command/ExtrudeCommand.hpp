#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Entity.hpp>

namespace nc::cmd {

/// Command to extrude a given face entity by a specified height.
class ExtrudeCommand : public ICommand {
   public:
    explicit ExtrudeCommand(Entity face, double height);

    void Execute(Registry& registry, GeometrySystem& geom) override;

    Entity GetFace() const;
    double GetHeight() const;

   private:
    Entity m_Face{INVALID_ENTITY};
    double m_Height{0.0};
};

}  // namespace nc
