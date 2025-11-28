#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Entity.hpp>

namespace nc::cmd {

/**
 * \brief Command to extrude a given face entity by a specified height.
 *
 * This command takes a `FaceComponent` entity and a `height` and uses the
 * `GeometrySystem` to create a `BodyComponent` representing the extruded solid.
 * On undo, it removes the created `BodyComponent` and `NameComponent`.
 */
class ExtrudeCommand : public ICommand {
   public:
    /**
     * \brief Constructs an ExtrudeCommand.
     * \param face The entity ID of the face to be extruded.
     * \param height The extrusion distance.
     */
    explicit ExtrudeCommand(domain::Entity face, double height);

    /**
     * \brief Executes the extrusion operation.
     * Creates a new body entity by extruding the specified face.
     * \param registry The central ECS registry.
     * \param geom The geometry system for performing the extrusion.
     */
    void Execute(domain::Registry& registry, domain::GeometrySystem& geom) override;

    /**
     * \brief Undoes the extrusion operation.
     * Removes the created body component and its name component from the registry.
     * \param registry The central ECS registry.
     * \param geom The geometry system (not directly used in Undo for this command).
     */
    void Undo(domain::Registry& registry, domain::GeometrySystem& geom) override;

    /**
     * \brief Returns the entity ID of the face being extruded.
     * \return The face entity ID.
     */
    domain::Entity GetFace() const;

    /**
     * \brief Returns the extrusion height.
     * \return The height value.
     */
    double GetHeight() const;
    
    /**
     * \brief Returns a human-readable name for the command.
     * \return "Extrude"
     */
    std::string GetName() const override { return "Extrude"; }

   private:
    domain::Entity m_Face{domain::INVALID_ENTITY}; ///< The entity ID of the face to extrude.
    double m_Height{0.0}; ///< The extrusion height.
    domain::Entity m_ResultEntity{domain::INVALID_ENTITY}; ///< The entity ID of the body created by this command.
};

}  // namespace nc::cmd
