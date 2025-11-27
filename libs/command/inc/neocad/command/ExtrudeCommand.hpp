#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Entity.hpp>

namespace nc::cmd {

/// Command to extrude a given face entity by a specified height.
class ExtrudeCommand : public ICommand {
   public:
    explicit ExtrudeCommand(domain::Entity face, double height);

    void Execute(domain::Registry& registry, domain::GeometrySystem& geom) override;
    void Undo(domain::Registry& registry, domain::GeometrySystem& geom) override;

    domain::Entity GetFace() const;
    double GetHeight() const;

   private:
    domain::Entity m_Face{domain::INVALID_ENTITY};
    double m_Height{0.0};
    domain::Entity m_ResultEntity{domain::INVALID_ENTITY};
};

}  // namespace nc::cmd
