#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Types.hpp>
#include <vector>

namespace nc::cmd {

/// Command to insert a parametric cylinder into the registry.

class InsertCylinderCommand : public ICommand {

public:

    /// \brief Constructs an InsertCylinderCommand.

    /// \param radius Initial radius of the cylinder.

    /// \param height Initial height of the cylinder.

    /// \param uiPosition 2D position for the UI node in the graph editor.

    InsertCylinderCommand(double radius, double height, domain::vec2 uiPosition);



    void Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;

    void Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;

    

    std::string GetName() const override { return "Insert Cylinder"; }



private:

    double m_Radius;

    double m_Height;

    domain::vec2 m_UIPosition;

    domain::EntityID m_CylinderEntityId{domain::INVALID_ENTITY};
    std::vector<domain::EntityID> m_ExpressionIds;
};

}  // namespace nc::cmd
