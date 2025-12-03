#pragma once
#include <ontoflow/editor/ITool.hpp>

namespace of::editor {

/**
 * \brief Editor tool for inserting single point entities into the ECS.
 *
 * On a mouse click, this tool creates a new entity with a `PositionComponent`
 * at the clicked world location and a `NameComponent`.
 */
class InsertPointTool : public ITool {
   public:
    /**
     * \brief Called when the tool becomes active (not used for this tool).
     * \param ctx The ToolContext.
     */
    void OnEnter(ToolContext&) override {
    }
    /**
     * \brief Called when the tool becomes inactive (not used for this tool).
     * \param ctx The ToolContext.
     */
    void OnExit(ToolContext&) override {
    }

    /**
     * \brief Handles input events for the tool.
     * Processes left mouse clicks to insert a point.
     * \param ev The input event to process.
     * \param ctx The ToolContext for accessing core services.
     * \return True if the event was consumed by the tool, false otherwise.
     */
    bool OnInput(const InputEvent& ev, ToolContext& ctx) override;

    /**
     * \brief Returns a human-readable name for the command.
     * \return "Insert Point"
     */
    std::string GetName() const override { return "Insert Point"; }
};

}  // namespace of::editor
