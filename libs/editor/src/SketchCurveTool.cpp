#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/editor/InputEvent.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/editor/ToolContext.hpp>

using namespace nc::domain;

namespace nc::editor {

void SketchCurveTool::OnEnter(ToolContext&) {
    m_Points.clear();
    m_WaitingSecondPoint = false;
    m_LastPreviewLine = INVALID_ENTITY;  // <- für Live-Preview
}

void SketchCurveTool::OnExit(ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();

    // Preview-Linie entfernen (falls vorhanden)
    if (m_LastPreviewLine != INVALID_ENTITY) {
        reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
        m_LastPreviewLine = INVALID_ENTITY;
    }

    m_Points.clear();
    m_WaitingSecondPoint = false;
}

void SketchCurveTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();

    // ----------------------- MOUSE MOVE = Preview -----------------------
    if (auto* m = AsMouseMove(ev)) {
        if (!m_Points.empty()) {
            glm::vec3 world = ctx.GetCamera()->ScreenToWorld(m->position.x, m->position.y);

            // 1) Update / create preview point
            if (m_PreviewPoint == INVALID_ENTITY) {
                m_PreviewPoint = reg.CreateEntity();
                reg.AddComponent(m_PreviewPoint, PositionComponent{world});
            } else {
                reg.GetComponent<PositionComponent>(m_PreviewPoint)->position = world;
            }

            // 2) Remove old rubberband preview line
            if (m_LastPreviewLine != INVALID_ENTITY) {
                reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
            }

            // 3) Create new rubberband preview line
            Entity a = m_Points.back();
            Entity b = m_PreviewPoint;
            m_LastPreviewLine = reg.CreateEntity();
            reg.AddComponent(m_LastPreviewLine, EdgeComponent{a, b});
        }
        return;
    }

    // ----------------------- MOUSE CLICK = FINAL POINT -----------------------
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            glm::vec3 world = ctx.GetCamera()->ScreenToWorld(m->position.x, m->position.y);

            // 1) new real point
            Entity p = reg.CreateEntity();
            reg.AddComponent(p, PositionComponent{world});
            m_Points.push_back(p);

            // 2) make PERSISTENT line (from last point)
            if (m_Points.size() >= 2) {
                Entity line = reg.CreateEntity();
                reg.AddComponent(line, EdgeComponent{m_Points[m_Points.size() - 2], m_Points.back()});
            }

            // 3) clear preview AFTER persistent line created
            if (m_LastPreviewLine != INVALID_ENTITY) {
                reg.RemoveComponent<EdgeComponent>(m_LastPreviewLine);
                m_LastPreviewLine = INVALID_ENTITY;
                m_PreviewPoint = INVALID_ENTITY;
            }

            // --- Mode: Single Line?
            if (m_Mode == CurveMode::Line) {
                if (!m_WaitingSecondPoint)
                    m_WaitingSecondPoint = true;
                else {
                    OnExit(ctx);  // already created line above!
                }
            }
            return;
        }
    }

    // ----------------------- KEY HANDLING -----------------------
    if (auto* k = AsKey(ev)) {
        if (k->code == KeyCode::Escape) {  // cancel but KEEP lines
            if (m_Mode == CurveMode::Polyline)
                FinalizePolyline(ctx);
            OnExit(ctx);
            return;
        }

        if (k->code == KeyCode::Enter) {  // CLOSE for face
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
    // Edges? Optional: Query all lines between these points
    reg.AddComponent(face, fc);
}

}  // namespace nc::editor
