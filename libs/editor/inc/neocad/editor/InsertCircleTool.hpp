#pragma once
#include <neocad/domain/Entity.hpp>
#include <neocad/editor/ITool.hpp>
#include <optional>

namespace nc::editor {

/**
 * \brief Editor tool for inserting circle entities into the ECS.
 *
 * This tool allows the user to define a circle by two mouse clicks:
 * the first click defines the center, and the second click defines a point
 * on the circumference, from which the radius is calculated.
 */
class InsertCircleTool : public ITool {
   public:
    /**
     * \brief Called when the tool becomes active.
     * Resets any internal state, such as pending center point.
     * \param ctx The ToolContext for accessing core services.
     */
    void OnEnter(ToolContext& ctx) override;

    /**
     * \brief Called when the tool becomes inactive.
     * Cleans up any pending state or temporary entities.
     * \param ctx The ToolContext for accessing core services.
     */
    void OnExit(ToolContext& ctx) override;

    /**
     * \brief Handles input events for the tool.
     * Processes mouse clicks to define the circle's center and radius.
     * \param event The input event to process.
     * \param ctx The ToolContext for accessing core services.
     * \return True if the event was consumed by the tool, false otherwise.
     */
    bool OnInput(const InputEvent& event, ToolContext& ctx) override;

    /**
     * \brief Updates the tool's state (not used for this tool).
     * \param ctx The ToolContext.
     * \param dt Time delta since last frame.
     */
    void Update(ToolContext& /*ctx*/, double /*dt*/) override {
    }

    /**
     * \brief Returns a human-readable name for the command.
     * \return "Insert Circle"
     */
    std::string GetName() const override { return "Insert Circle"; }

   private:
    std::optional<domain::Entity> m_Center;  ///< Stores the entity ID of the temporarily created center point.
};

}  // namespace nc::editor
