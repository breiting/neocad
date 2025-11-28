#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Types.hpp>
#include <vector>

namespace nc::cmd {

/// Command to insert a parametric cylinder into the registry.
class InsertCylinderCommand : public ICommand {
   public:
    InsertCylinderCommand(double radius, double height);

    void Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;
    void Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;

    std::string GetName() const override {
        return "Insert Cylinder";
    }

   private:
    double m_Radius;
    double m_Height;

    domain::EntityID m_CylinderEntityId{domain::INVALID_ENTITY};
    std::vector<domain::EntityID> m_ExpressionIds;
};

}  // namespace nc::cmd
