#pragma once
#include <neocad/editor/InputEvent.hpp>
#include <string>

namespace nc::editor {

class ToolContext; // Forward declaration

/**
 * \brief Base interface for all editor tools.
 *
 * Tools are mode-local state machines that react to input and mutate the ECS.
 * They encapsulate specific editing operations, such as inserting geometry
 * or performing selections.
 */
class ITool {
   public:
    /**
     * \brief Virtual destructor to ensure proper cleanup of derived tool classes.
     */
    virtual ~ITool() = default;

    /**
     * \brief Called when the tool becomes active (e.g., a mode switch).
     * Implementations should reset their internal state here.
     * \param ctx The ToolContext for accessing core services.
     */
    virtual void OnEnter(ToolContext&) {
    }

    /**
     * \brief Called when the tool becomes inactive.
     * Implementations should clean up any temporary entities or state.
     * \param ctx The ToolContext for accessing core services.
     */
    virtual void OnExit(ToolContext& ctx) = 0;

    /**
     * \brief Handles an input event.
     * \param ev The input event to process.
     * \param ctx The ToolContext for accessing core services.
     * \return True if the event was consumed by the tool, false otherwise.
     */
    virtual bool OnInput(const InputEvent& ev, ToolContext& ctx) = 0;

    /**
     * \brief Updates the tool's internal state (e.g., damping, interpolation).
     * This is called once per frame while the tool is active.
     * \param ctx The ToolContext.
     * \param dt Time delta since last frame.
     */
    virtual void Update(ToolContext& /*ctx*/, double /*dt*/) {}

    /**
     * \brief Provides a human-readable name for the tool.
     * This can be used for UI elements (e.g., tooltips or status bar).
     * \return A string representing the tool's name.
     */
    virtual std::string GetName() const = 0;
};

}  // namespace nc::editor
