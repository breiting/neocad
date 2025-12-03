#pragma once
#include <chrono>
#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/editor/ITool.hpp>
#include <ontoflow/editor/Mode.hpp> // Not strictly needed in header, but for context
#include <vector>

namespace of::editor {

/**
 * \brief Editor tool for sketching curves, polylines, or faces.
 *
 * This tool allows interactive creation of geometry by placing points
 * and connecting them. It supports different modes for creating lines,
 * polylines, or closed faces. It uses the command stack for undo/redo.
 */
class SketchCurveTool : public ITool {
   public:
    /**
     * \brief Defines the mode of operation for the SketchCurveTool.
     */
    enum class CurveMode {
        Line,     ///< Create a single line segment (two points).
        Polyline, ///< Create a sequence of connected line segments.
        Face      ///< Create a closed face from connected line segments.
    };

    /**
     * \brief Constructs a SketchCurveTool.
     * \param mode The desired curve creation mode.
     */
    SketchCurveTool(CurveMode mode) : m_Mode(mode) {
    }

    /**
     * \brief Called when the tool becomes active.
     * Resets internal state and records the current command stack index for potential undo.
     * \param ctx The ToolContext for accessing core services.
     */
    void OnEnter(ToolContext& ctx) override;

    /**
     * \brief Called when the tool becomes inactive.
     * Cleans up preview geometry and undoes any uncommitted operations if not finalized.
     * \param ctx The ToolContext for accessing core services.
     */
    void OnExit(ToolContext& ctx) override;

    /**
     * \brief Handles input events for the tool.
     * Processes mouse clicks to place points, mouse moves for preview, and keyboard for finalization/cancel.
     * \param ev The input event to process.
     * \param ctx The ToolContext for accessing core services.
     * \return True if the event was consumed by the tool, false otherwise.
     */
    bool OnInput(const InputEvent& ev, ToolContext& ctx) override;

    /**
     * \brief Provides a human-readable name for the tool.
     * \return A string representing the tool's name.
     */
    std::string GetName() const override {
        switch(m_Mode) {
            case CurveMode::Line: return "Sketch Line";
            case CurveMode::Polyline: return "Sketch Polyline";
            case CurveMode::Face: return "Sketch Face";
        }
        return "Sketch Curve"; // Should not be reached
    }

   private:
    CurveMode m_Mode; ///< The current curve creation mode.
    std::vector<of::domain::Entity> m_Points; ///< List of placed point entities for the current sketch.

    of::domain::Entity m_PreviewPoint = domain::INVALID_ENTITY; ///< Entity for the dynamically updated preview point.
    of::domain::Entity m_LastPreviewLine = domain::INVALID_ENTITY; ///< Entity for the dynamically updated preview line.
    bool m_WaitingSecondPoint = false; ///< Flag for Line mode to track if waiting for the second point.
    
    size_t m_StartCmdIndex{0}; ///< Command stack index at the start of this tool's session.

    using Clock = std::chrono::steady_clock;
    Clock::time_point m_LastClickTime = Clock::now(); ///< Timestamp of the last mouse click for debouncing.

    void FinalizePolyline(ToolContext& ctx);
    void FinalizeFace(ToolContext& ctx);
};

}  // namespace of::editor