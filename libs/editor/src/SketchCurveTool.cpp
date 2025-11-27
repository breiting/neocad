#include <neocad/domain/Registry.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/editor/ToolContext.hpp>

using namespace nc::domain;
using Clock = std::chrono::steady_clock;

namespace nc::editor {

constexpr double DEBOUNCE_TIME = 0.05;

// -------------------------------------------------
void SketchCurveTool::OnEnter(ToolContext&) {
    m_Points.clear();
    m_WaitingSecondPoint = false;
    m_PreviewPoint = INVALID_ENTITY;
    m_LastPreviewLine = INVALID_ENTITY;
}

// -------------------------------------------------
void SketchCurveTool::OnExit(ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();

    if (m_LastPreviewLine != INVALID_ENTITY) {
        reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
    }
    m_PreviewPoint = INVALID_ENTITY;
    m_LastPreviewLine = INVALID_ENTITY;

    m_Points.clear();
    m_WaitingSecondPoint = false;
}

// -------------------------------------------------
void SketchCurveTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();

    // ----- MOUSE MOVE → Preview -----
    if (auto* m = AsMouseMove(ev)) {
        if (m_Points.empty())
            return;

        double elapsed = std::chrono::duration<double>(Clock::now() - m_LastClickTime).count();
        if (elapsed < DEBOUNCE_TIME)
            return;

        glm::vec3 world = ctx.GetCamera()->ScreenToWorld(m->position.x, m->position.y);

        if (m_PreviewPoint == INVALID_ENTITY) {
            m_PreviewPoint = reg.CreateEntity();
            reg.AddComponent(m_PreviewPoint, PositionComponent{world});
        } else {
            reg.GetComponent<PositionComponent>(m_PreviewPoint)->position = world;
        }

        if (m_LastPreviewLine != INVALID_ENTITY) {
            reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
        }

        m_LastPreviewLine = reg.CreateEntity();
        reg.AddComponent(m_LastPreviewLine, EdgeComponent{m_Points.back(), m_PreviewPoint});

        return;
    }

    // ----- LEFT MOUSE → Commit Point -----
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            m_LastClickTime = Clock::now();

            glm::vec3 world = ctx.GetCamera()->ScreenToWorld(m->position.x, m->position.y);

            // Reuse preview as final point if exists
            Entity p;
            if (m_PreviewPoint != INVALID_ENTITY) {
                p = m_PreviewPoint;
                reg.GetComponent<PositionComponent>(p)->position = world;
                m_PreviewPoint = INVALID_ENTITY;
            } else {
                p = reg.CreateEntity();
                reg.AddComponent(p, PositionComponent{world});
            }
            m_Points.push_back(p);

            // Create persistent line from last real point
            if (m_Points.size() >= 2) {
                CreateLine(m_Points[m_Points.size() - 2], m_Points.back(), reg);
            }

            if (m_LastPreviewLine != INVALID_ENTITY) {
                reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
                m_LastPreviewLine = INVALID_ENTITY;
            }

            if (m_Mode == CurveMode::Line) {
                if (!m_WaitingSecondPoint) {
                    m_WaitingSecondPoint = true;
                } else {
                    OnExit(ctx);
                }
            }
            return;
        }
    }

    // ----- KEYS -----
    if (auto* k = AsKey(ev)) {
        if (k->code == KeyCode::Escape) {
            if (m_Mode == CurveMode::Polyline)
                FinalizePolyline(ctx);
            OnExit(ctx);
            return;
        }
        if (k->code == KeyCode::Enter) {
            if (m_Mode == CurveMode::Face)
                FinalizeFace(ctx);
            OnExit(ctx);
            return;
        }
    }
}

// -------------------------------------------------
void SketchCurveTool::CreateLine(Entity a, Entity b, Registry& reg) {
    Entity line = reg.CreateEntity();
    reg.AddComponent(line, EdgeComponent{a, b});
}

// -------------------------------------------------
void SketchCurveTool::FinalizePolyline(ToolContext& ctx) {
    if (m_Points.size() < 2)
        return;
    auto& reg = ctx.GetRegistry();

    for (size_t i = 0; i + 1 < m_Points.size(); ++i) {
        CreateLine(m_Points[i], m_Points[i + 1], reg);
    }
}

// -------------------------------------------------
void SketchCurveTool::FinalizeFace(ToolContext& ctx) {
    if (m_Points.size() < 3)
        return;
    auto& reg = ctx.GetRegistry();

    FinalizePolyline(ctx);
    CreateLine(m_Points.back(), m_Points.front(), reg);

    Entity face = reg.CreateEntity();
    FaceComponent fc;
    fc.vertices = m_Points;
    reg.AddComponent(face, fc);
}

}  // namespace nc::editor
