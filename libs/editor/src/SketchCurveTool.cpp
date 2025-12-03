#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/editor/SketchCurveTool.hpp>
#include <ontoflow/editor/ToolContext.hpp>
#include <ontoflow/editor/Editor.hpp> // Required for SetMode
#include <ontoflow/command/BasicCommands.hpp>
#include <ontoflow/core/Logger.hpp> // Include Logger

using namespace of::domain;
using Clock = std::chrono::steady_clock;

namespace of::editor {

constexpr double DEBOUNCE_TIME = 0.05; // Time in seconds to debounce mouse clicks

/**
 * \brief Called when the tool becomes active.
 * Resets internal state and records the current command stack index for potential undo.
 * \param ctx The ToolContext for accessing core services.
 */
void SketchCurveTool::OnEnter(ToolContext& ctx) {
    m_Points.clear();
    m_WaitingSecondPoint = false;
    m_PreviewPoint = INVALID_ENTITY;
    m_LastPreviewLine = INVALID_ENTITY;
    
    // Record the start of this session on the command stack
    m_StartCmdIndex = ctx.GetCommandStack().GetCurrentIndex();
    LOG(Info) << "SketchCurveTool: Entered mode " << static_cast<int>(m_Mode) << ", StartCmdIndex: " << m_StartCmdIndex;
}

/**
 * \brief Called when the tool becomes inactive.
 * Cleans up preview geometry and undoes any uncommitted operations if not finalized.
 * \param ctx The ToolContext for accessing core services.
 */
void SketchCurveTool::OnExit(ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();
    LOG(Info) << "SketchCurveTool: Exiting. Points captured: " << m_Points.size();

    // 1. Clean up purely visual preview elements (not part of the command stack)
    if (m_LastPreviewLine != INVALID_ENTITY) {
        if (reg.HasComponent<EdgeComponent>(m_LastPreviewLine)) {
            reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
        }
    }
    if (m_PreviewPoint != INVALID_ENTITY) {
        if (reg.HasComponent<PositionComponent>(m_PreviewPoint)) {
            reg.RemoveComponent<PositionComponent>(m_PreviewPoint);
        }
    }
    m_PreviewPoint = INVALID_ENTITY;
    m_LastPreviewLine = INVALID_ENTITY;

    // 2. If we didn't finish successfully (e.g. Escape was pressed), undo everything done in this session.
    // We check if m_Points is NOT empty, which implies the operation was not finalized.
    if (!m_Points.empty()) {
         LOG(Info) << "SketchCurveTool: Cancelling operation. Undoing commands to index " << m_StartCmdIndex;
         ctx.GetCommandStack().UndoTo(m_StartCmdIndex);
    }

    m_Points.clear();
    m_WaitingSecondPoint = false;
    LOG(Info) << "SketchCurveTool: Exited.";
}

/**
 * \brief Handles input events for the tool.
 * Processes mouse clicks to place points, mouse moves for preview, and keyboard for finalization/cancel.
 * \param ev The input event to process.
 * \param ctx The ToolContext for accessing core services.
 * \return True if the event was consumed by the tool, false otherwise.
 */
bool SketchCurveTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();

    // ----- MOUSE MOVE → Preview -----
    if (auto* m = AsMouseMove(ev)) {
        // Only update preview if we've already placed at least one point
        if (m_Points.empty())
            return false; // Event not handled for mode change

        // Debounce mouse move to avoid excessive updates
        double elapsed = std::chrono::duration<double>(Clock::now() - m_LastClickTime).count();
        if (elapsed < DEBOUNCE_TIME)
            return false; // Event handled (debounced), no mode change

        ICamera* cam = ctx.GetCamera();
        if (!cam) {
            LOG(Error) << "SketchCurveTool: No active camera for screen-to-world conversion.";
            return false; // Event not handled for mode change
        }
        glm::vec3 world = cam->ScreenToWorld(m->position.x, m->position.y);

        // Create or update the preview point
        if (m_PreviewPoint == INVALID_ENTITY) {
            m_PreviewPoint = reg.CreateEntity();
            reg.AddComponent(m_PreviewPoint, PositionComponent{world});
        } else {
            reg.AddComponent(m_PreviewPoint, PositionComponent{world}); // Update, triggers dirty flag
        }

        // Remove old preview line if exists
        if (m_LastPreviewLine != INVALID_ENTITY) {
            if (reg.HasComponent<EdgeComponent>(m_LastPreviewLine)) {
                reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
            }
        }

        // Create new preview line from last committed point to current preview point
        m_LastPreviewLine = reg.CreateEntity();
        reg.AddComponent(m_LastPreviewLine, EdgeComponent{m_Points.back(), m_PreviewPoint});

        return false; // Event handled (preview updated), no mode change
    }

    // ----- LEFT MOUSE → Commit Point -----
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            // Debounce click to prevent double-clicks
            double elapsed = std::chrono::duration<double>(Clock::now() - m_LastClickTime).count();
            if (elapsed < DEBOUNCE_TIME) {
                return true; // Event handled (debounced), no mode change
            }
            m_LastClickTime = Clock::now();

            ICamera* cam = ctx.GetCamera();
            if (!cam) {
                LOG(Error) << "SketchCurveTool: No active camera for screen-to-world conversion.";
                return false; // Event not handled for mode change
            }
            glm::vec3 world = cam->ScreenToWorld(m->position.x, m->position.y);
            
            // Create new point entity and add it to registry via command
            Entity p = reg.CreateEntity();
            auto pCmd = std::make_unique<cmd::CreateComponentCommand<PositionComponent>>(p, PositionComponent{world});
            ctx.GetCommandStack().Push(std::move(pCmd));
            m_Points.push_back(p);
            LOG(Info) << "SketchCurveTool: Added point " << p << " at (" << world.x << ", " << world.y << ", " << world.z << ")";
            
            // If more than one point, create a persistent line segment via command
            if (m_Points.size() >= 2) {
                Entity l = reg.CreateEntity();
                auto lCmd = std::make_unique<cmd::CreateComponentCommand<EdgeComponent>>(l, EdgeComponent{m_Points[m_Points.size() - 2], m_Points.back()});
                ctx.GetCommandStack().Push(std::move(lCmd));
                LOG(Info) << "SketchCurveTool: Added line " << l << " between " << m_Points[m_Points.size() - 2] << " and " << m_Points.back();
            }

            // Clean up preview elements which will be replaced by persistent ones
            if (m_LastPreviewLine != INVALID_ENTITY) {
                if (reg.HasComponent<EdgeComponent>(m_LastPreviewLine)) {
                    reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
                }
                m_LastPreviewLine = INVALID_ENTITY;
            }
            if (m_PreviewPoint != INVALID_ENTITY) {
                 if (reg.HasComponent<PositionComponent>(m_PreviewPoint)) {
                    reg.RemoveComponent<PositionComponent>(m_PreviewPoint);
                }
                m_PreviewPoint = INVALID_ENTITY;
            }

            // Handle specific CurveMode logic
            if (m_Mode == CurveMode::Line) {
                if (!m_WaitingSecondPoint) {
                    m_WaitingSecondPoint = true; // Waiting for the second point to complete the line
                } else {
                    // Finished single line - tool completes
                    m_Points.clear(); // Mark as success so OnExit doesn't Undo
                    ctx.GetEditor()->SetMode(EditorMode::Normal); // Switch back to normal mode
                    return true;
                }
            }
            return true; // Event handled, no mode change
        }
    }

    // ----- KEYBOARD INPUT -----
    if (auto* k = AsKey(ev)) {
        if (!k->pressed) return false;

        // ESCAPE key cancels the current sketching operation
        if (k->code == KeyCode::Escape) {
            LOG(Info) << "SketchCurveTool: ESC pressed. Cancelling current sketch.";
            ctx.GetEditor()->SetMode(EditorMode::Normal); // Switch back to normal mode
            return true;
        }

        // ENTER key finalizes the curve/face
        if (k->code == KeyCode::Enter) {
            LOG(Info) << "SketchCurveTool: ENTER pressed. Finalizing sketch.";
            if (m_Points.size() >= 2) { // Need at least two points for polyline/face
                if (m_Mode == CurveMode::Polyline) {
                    FinalizePolyline(ctx);
                } else if (m_Mode == CurveMode::Face) {
                    FinalizeFace(ctx);
                }
            } else {
                LOG(Warn) << "SketchCurveTool: Not enough points to finalize current sketch.";
            }
            m_Points.clear(); // Mark as success so OnExit doesn't Undo
            ctx.GetEditor()->SetMode(EditorMode::Normal); // Switch back to normal mode
            return true;
        }
    }
    
    return false; // Event not handled for mode change
}

/**
 * \brief Finalizes a polyline sketch.
 * For a polyline, the segments are already committed. This just clears state.
 * \param ctx The ToolContext.
 */
void SketchCurveTool::FinalizePolyline(ToolContext& /*ctx*/) {
    LOG(Info) << "SketchCurveTool: Finalizing Polyline.";
    // For a polyline, segments are added as points are placed. No extra action needed here.
}

/**
 * \brief Finalizes a face sketch.
 * Closes the loop and creates a FaceComponent.
 * \param ctx The ToolContext.
 */
void SketchCurveTool::FinalizeFace(ToolContext& ctx) {
    if (m_Points.size() < 3) {
        LOG(Warn) << "SketchCurveTool: Cannot finalize face, fewer than 3 points.";
        return; // Not enough points for a face
    }
    auto& reg = ctx.GetRegistry();

    // Close the loop with an additional edge from last point to first point
    Entity l = reg.CreateEntity();
    auto lCmd = std::make_unique<cmd::CreateComponentCommand<EdgeComponent>>(l, EdgeComponent{m_Points.back(), m_Points.front()});
    ctx.GetCommandStack().Push(std::move(lCmd));
    LOG(Info) << "SketchCurveTool: Closing face loop with edge " << l << " from " << m_Points.back() << " to " << m_Points.front();

    // Create the FaceComponent
    Entity face = reg.CreateEntity();
    FaceComponent fc;
    fc.vertices = m_Points; 
    // The Edges vector in FaceComponent is usually populated by GeometrySystem when creating face entities based on a profile.
    // For now, only vertices are stored. This might need refinement based on how FaceComponent is truly used.
    
    auto fCmd = std::make_unique<cmd::CreateComponentCommand<FaceComponent>>(face, fc);
    ctx.GetCommandStack().Push(std::move(fCmd));
    LOG(Info) << "SketchCurveTool: Finalized Face entity " << face << " with " << m_Points.size() << " vertices.";
}

}  // namespace of::editor