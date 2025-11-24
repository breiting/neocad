#include <glm/glm.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertCircleTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <variant>

namespace nc {

void InsertCircleTool::OnInput(const InputEvent& event, ToolContext& ctx) {
    if (event.type != InputEventType::MouseButton)
        return;
    const auto* mb = std::get_if<MouseButtonEvent>(&event.data);
    if (!mb || !mb->pressed)
        return;

    auto& reg = ctx.GetRegistry();

    // First click → store center point
    if (!m_Center.has_value()) {
        PositionComponent pc;
        pc.position = glm::dvec3(mb->x, mb->y, 0.0);
        Entity center = reg.CreateEntity();
        reg.AddComponent(center, pc);

        m_Center = center;
        return;
    }

    // Second click → compute radius and create component
    Entity center = *m_Center;
    PositionComponent* pCenter = reg.GetComponent<PositionComponent>(center);
    if (!pCenter)
        return;

    double dx = mb->x - pCenter->position.x;
    double dy = mb->y - pCenter->position.y;
    double radius = std::sqrt(dx * dx + dy * dy);

    RadiusComponent rc;
    rc.radius = radius;
    reg.AddComponent(center, rc);

    // DONE – reset for next circle
    m_Center.reset();
}

}  // namespace nc
