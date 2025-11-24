#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/editor/InputEvent.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/editor/ToolContext.hpp>

namespace nc {

// -------------------------------------------------
void SketchCurveTool::OnEnter(ToolContext&) {
    m_Points.clear();
    m_WaitingSecondPoint = false;
}

// -------------------------------------------------
void SketchCurveTool::OnExit(ToolContext&) {
    m_Points.clear();
    m_WaitingSecondPoint = false;
}

// -------------------------------------------------
void SketchCurveTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    auto& reg = ctx.GetRegistry();
    auto& geom = ctx.GetGeometrySystem();

    if (auto* m = AsMouseButton(ev)) {
        if (!(m->button == MouseButton::Left))
            return;

        Entity p = reg.CreateEntity();
        reg.AddComponent(p, PositionComponent{{m->position.x, m->position.y, 0.0}});
        m_Points.push_back(p);

        // --- MODE: LINE ---
        if (m_Mode == CurveMode::Line) {
            if (!m_WaitingSecondPoint) {
                m_WaitingSecondPoint = true;
            } else {
                CreateLine(m_Points[0], m_Points[1], reg);
                OnExit(ctx);
            }
        }
        return;
    }

    if (auto* k = AsKey(ev)) {
        if (k->code == KeyCode::Escape) {
            // finish polyline WITHOUT closing
            if (m_Mode == CurveMode::Polyline) {
                FinalizePolyline(ctx);
            }
            OnExit(ctx);
            return;
        }

        if (k->code == KeyCode::Enter) {
            // finish face WITH closing loop
            if (m_Mode == CurveMode::Face) {
                FinalizeFace(ctx);
            }
            OnExit(ctx);
            return;
        }
    }
}

// -------------------------------------------------
void SketchCurveTool::CreateLine(Entity a, Entity b, Registry& reg) {
    Entity line = reg.CreateEntity();
    reg.AddComponent(line, LineComponent{a, b});
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
    auto& geom = ctx.GetGeometrySystem();

    FinalizePolyline(ctx);
    // CLOSE LOOP
    CreateLine(m_Points.back(), m_Points.front(), reg);

    // register as FaceComponent
    Entity face = reg.CreateEntity();
    FaceComponent fc;
    fc.edges = m_Points;
    reg.AddComponent(face, fc);
}

}  // namespace nc
