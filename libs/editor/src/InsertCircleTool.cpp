#include <glm/glm.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertCircleTool.hpp>
#include <neocad/editor/ToolContext.hpp>

namespace nc {

void InsertCircleTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (ev.IsLeftMouseClick()) {
        const auto& m = ev.AsMouseButton();

        auto& reg = ctx.GetRegistry();

        // First click → store center point
        if (!m_Center.has_value()) {
            PositionComponent pc;
            pc.position = glm::dvec3(m.x, m.y, 0.0);
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

        double dx = m.x - pCenter->position.x;
        double dy = m.y - pCenter->position.y;
        double radius = std::sqrt(dx * dx + dy * dy);

        RadiusComponent rc;
        rc.radius = radius;
        reg.AddComponent(center, rc);

        // DONE – reset for next circle
        m_Center.reset();
    }
}

}  // namespace nc
