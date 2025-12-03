#pragma once
#include <ontoflow/command/ICommand.hpp>
#include <ontoflow/domain/Types.hpp>
#include <vector>

namespace of::cmd {

/// Command to insert a parametric box into the registry.

class InsertBoxCommand : public ICommand {

public:

    /// \brief Constructs an InsertBoxCommand.

    /// \param width Initial width of the box.

    /// \param length Initial length of the box.

    /// \param height Initial height of the box.

    /// \param uiPosition 2D position for the UI node in the graph editor.

    InsertBoxCommand(double width, double length, double height, domain::vec2 uiPosition);



    void Execute(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;

    void Undo(domain::Registry& registry, domain::GeometrySystem& geometrySystem) override;

    

    std::string GetName() const override { return "Insert Box"; }



private:

    double m_Width;

    double m_Length;

    double m_Height;

    domain::vec2 m_UIPosition;

    domain::EntityID m_BoxEntityId{domain::INVALID_ENTITY};
    std::vector<domain::EntityID> m_ExpressionIds;
};

}  // namespace of::cmd
