#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertLineTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <variant>

namespace nc {

void InsertLineTool::OnEnter(ToolContext&) {
    m_PendingStartPoint.reset();
}

void InsertLineTool::OnExit(ToolContext&) {
    m_PendingStartPoint.reset();
}

void InsertLineTool::OnInput(const InputEvent& event, ToolContext& ctx) {
    if (event.type != InputEventType::MouseButton)
        return;

    const auto* mb = std::get_if<MouseButtonEvent>(&event.data);
    if (!mb || !mb->pressed)
        return;

    auto& reg = ctx.GetRegistry();

    // Create a point at click position
    PositionComponent pc;
    pc.position = vec3(mb->x, mb->y, 0.0);
    Entity pointEntity = reg.CreateEntity();
    reg.AddComponent(pointEntity, pc);

    if (!m_PendingStartPoint.has_value()) {
        // This is the first point of the line
        m_PendingStartPoint = pointEntity;
    } else {
        // Second point: create line entity
        Entity start = *m_PendingStartPoint;
        Entity end = pointEntity;

        LineComponent lc;
        lc.p0 = start;
        lc.p1 = end;
        Entity lineEntity = reg.CreateEntity();
        reg.AddComponent(lineEntity, lc);

        // Reset for next segment
        m_PendingStartPoint = std::nullopt;
    }
}

}  // namespace nc
