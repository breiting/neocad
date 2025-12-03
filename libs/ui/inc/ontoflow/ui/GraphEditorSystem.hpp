#pragma once

#include <string>

#include "ontoflow/command/CommandStack.hpp"
#include "ontoflow/domain/Registry.hpp"
#include "ontoflow/domain/Types.hpp"
#include <glm/vec2.hpp>

// Forward declaration for ImGui context and ImNodes
namespace ImNodes {
using Context = void;
}

namespace of::ui {

/**
 * \brief System responsible for drawing the node graph editor.
 *
 * Manages the visualization and interaction with the parametric graph
 * using ImNodes. It handles rendering of parameter and feature nodes,
 * as well as the connections (links) between them.
 */
class GraphEditorSystem final {
   public:
    /**
     * \brief Constructs the GraphEditorSystem.
     * \param registry Reference to the ECS registry containing the model data.
     * \param commandStack Reference to the command stack for executing commands.
     */
    explicit GraphEditorSystem(domain::Registry& registry, of::cmd::CommandStack& commandStack);

    /**
     * \brief Draws the main graph editor panel.
     * Should be called inside an ImGui frame.
     */
    void DrawPanel();

    /**
     * \brief Toggles the visibility of the graph editor window.
     */
    void ToggleVisibility();

    /**
     * \brief Checks if the graph editor window is currently visible.
     * \return True if visible, false otherwise.
     */
    bool IsVisible() const {
        return m_IsVisible;
    }

   private:
    domain::Registry& m_Registry;
    of::cmd::CommandStack& m_CommandStack;
    bool m_IsVisible = true;

    // Helper to store position of newly created nodes
    glm::vec2 m_CurrentMouseGridPosition{0.0f, 0.0f};

    /**
     * \brief Draws a node representing a global parameter.
     * \param parameterId The EntityID of the global parameter.
     */
    void DrawParameterNode(domain::EntityID parameterId);

    /**
     * \brief Draws a node representing a geometric feature (Box, Cylinder, etc.).
     * \param featureId The EntityID of the feature.
     */
    void DrawFeatureNode(domain::EntityID featureId);

    /**
     * \brief Helper to draw an input pin (for expressions) or output pin.
     * \param expressionId The EntityID of the expression component (for inputs).
     * \param label The label to display next to the pin.
     * \param isInput True if this is an input pin (left side), false for output (right side).
     */
    void DrawPinAndInput(domain::EntityID expressionId, const std::string& label, bool isInput);

    /**
     * \brief Retrieves a robust title for a node given its EntityID.
     * \param entityId The EntityID of the node.
     * \return The name of the node, or a fallback if NameComponent is missing.
     */
    std::string GetNodeTitle(domain::EntityID entityId);

    /**
     * \brief Updates and returns the current mouse position in grid space.
     * \return The mouse position in the ImNodes grid coordinate system.
     */
    glm::vec2 GetCurrentMouseGridPosition();
};

}  // namespace of::ui
