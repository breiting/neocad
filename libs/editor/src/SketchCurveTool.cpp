#include <neocad/domain/Registry.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <neocad/command/BasicCommands.hpp>

using namespace nc::domain;
using Clock = std::chrono::steady_clock;

namespace nc::editor {

constexpr double DEBOUNCE_TIME = 0.05;

// -------------------------------------------------
void SketchCurveTool::OnEnter(ToolContext& ctx) {
    m_Points.clear();
    m_WaitingSecondPoint = false;
    m_PreviewPoint = INVALID_ENTITY;
    m_LastPreviewLine = INVALID_ENTITY;
    
    // Record the start of this session on the command stack
    m_StartCmdIndex = ctx.GetCommandStack().GetCurrentIndex();
}

// -------------------------------------------------
void SketchCurveTool::OnExit(ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();
    LOG(INFO) << "SketchCurveTool::OnExit - Points: " << m_Points.size();

    // 1. Clean up purely visual preview elements (not on stack)
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

    // 2. If we didn't finish successfully (e.g. Escape), undo everything done in this session.
    // We detect "finished" by checking if m_Points was cleared by Finalize* methods?
    // OR simpler: Finalize methods should commit, Cancel (Escape) calls Undo.
    // BUT OnExit is called in both cases.
    // Let's change logic: Explicitly call UndoTo if m_Points is NOT empty (meaning we are aborting).
    
    if (!m_Points.empty()) {
         LOG(INFO) << "Cancelling operation -> UndoTo " << m_StartCmdIndex;
         ctx.GetCommandStack().UndoTo(m_StartCmdIndex);
    }

    m_Points.clear();
    m_WaitingSecondPoint = false;
}

// -------------------------------------------------
bool SketchCurveTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();

    // ----- MOUSE MOVE → Preview -----
    if (auto* m = AsMouseMove(ev)) {
        if (m_Points.empty())
            return false; // Let camera handle move if we aren't sketching yet? No, if points exist, we are sketching.
        
        // Actually, if we are just moving mouse, we usually DON'T want to block camera orbit 
        // unless we are "dragging" something. 
        // But here we are just moving the preview.
        // The user might want to rotate view while sketching!
        // So we return FALSE for MouseMove, unless we strictly want to own it.
        // Let's return FALSE for MouseMove so Orbit works while sketching.
        
        double elapsed = std::chrono::duration<double>(Clock::now() - m_LastClickTime).count();
        if (elapsed < DEBOUNCE_TIME)
            return false;

        glm::vec3 world = ctx.GetCamera()->ScreenToWorld(m->position.x, m->position.y);

        if (m_PreviewPoint == INVALID_ENTITY) {
            m_PreviewPoint = reg.CreateEntity();
            reg.AddComponent(m_PreviewPoint, PositionComponent{world});
        } else {
            // Use AddComponent to overwrite and trigger rendering update (Dirty flag)
            reg.AddComponent(m_PreviewPoint, PositionComponent{world});
        }

        if (m_LastPreviewLine != INVALID_ENTITY) {
            reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
        }

        m_LastPreviewLine = reg.CreateEntity();
        reg.AddComponent(m_LastPreviewLine, EdgeComponent{m_Points.back(), m_PreviewPoint});

        return false; // Allow camera to see mouse move
    }

    // ----- LEFT MOUSE → Commit Point -----
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            // Debounce check
            double elapsed = std::chrono::duration<double>(Clock::now() - m_LastClickTime).count();
            if (elapsed < DEBOUNCE_TIME) {
                return true; // Ignore bouncing click
            }

            m_LastClickTime = Clock::now();

            glm::vec3 world = ctx.GetCamera()->ScreenToWorld(m->position.x, m->position.y);
            
            Entity p = reg.CreateEntity();
            auto pCmd = std::make_unique<cmd::CreateComponentCommand<PositionComponent>>(p, PositionComponent{world}, true);
            ctx.GetCommandStack().Push(std::move(pCmd));
            
            m_Points.push_back(p);

            // Create persistent line from last real point
            if (m_Points.size() >= 2) {
                Entity l = reg.CreateEntity();
                auto lCmd = std::make_unique<cmd::CreateComponentCommand<EdgeComponent>>(l, EdgeComponent{m_Points[m_Points.size() - 2], m_Points.back()}, true);
                ctx.GetCommandStack().Push(std::move(lCmd));
            }

            // Reset preview
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

            if (m_Mode == CurveMode::Line) {
                if (!m_WaitingSecondPoint) {
                    m_WaitingSecondPoint = true;
                } else {
                    // Finished Line
                    m_Points.clear(); // Clear points so OnExit doesn't Undo
                    OnExit(ctx);
                }
            }
            return true; // CONSUMED! Camera does NOT orbit.
        }
    }

    // ----- KEYS -----
    if (auto* k = AsKey(ev)) {
        if (k->code == KeyCode::Escape) {
            OnExit(ctx);
            return true; // Consumed
        }
        if (k->code == KeyCode::Enter) {
            if (m_Mode == CurveMode::Polyline)
                FinalizePolyline(ctx);
            else if (m_Mode == CurveMode::Face)
                FinalizeFace(ctx);
            
            OnExit(ctx);
            return true; // Consumed
        }
    }
    
    return false;
}

// -------------------------------------------------
void SketchCurveTool::CreateLine(Entity /*a*/, Entity /*b*/, Registry& /*reg*/) {
   // Helper unused now, logic moved to OnInput for stack support
}

// -------------------------------------------------
void SketchCurveTool::FinalizePolyline(ToolContext& /*ctx*/) {
    // Polyline is already built step-by-step. Just commit.
    m_Points.clear(); // Mark as success
}

// -------------------------------------------------
void SketchCurveTool::FinalizeFace(ToolContext& ctx) {
    if (m_Points.size() < 3) {
        m_Points.clear(); // Abort
        return;
    }
    auto& reg = ctx.GetRegistry();

    // Close the loop
    Entity l = reg.CreateEntity();
    auto lCmd = std::make_unique<cmd::CreateComponentCommand<EdgeComponent>>(l, EdgeComponent{m_Points.back(), m_Points.front()}, true);
    ctx.GetCommandStack().Push(std::move(lCmd));

    Entity face = reg.CreateEntity();
    FaceComponent fc;
    fc.vertices = m_Points; 
    // We need edge list too strictly speaking, but for now just vertices.
    // Re-collect edges? The commands created them. 
    // The FaceComponent stores Entity IDs.
    
    auto fCmd = std::make_unique<cmd::CreateComponentCommand<FaceComponent>>(face, fc, true);
    ctx.GetCommandStack().Push(std::move(fCmd));

    m_Points.clear(); // Mark as success
}

}  // namespace nc::editor