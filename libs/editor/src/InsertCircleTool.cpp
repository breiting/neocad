#include <glm/glm.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InputEvent.hpp>
#include <neocad/editor/InsertCircleTool.hpp>
#include <neocad/editor/ToolContext.hpp>

using namespace nc::domain;

namespace nc::editor {

void InsertCircleTool::OnEnter(ToolContext& /*ctx*/) {
    // Optionally reset state
    m_Center.reset();
}

void InsertCircleTool::OnExit(ToolContext& /*ctx*/) {
    // Cleanup if mid-operation?
    m_Center.reset();
}

bool InsertCircleTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            auto& reg = ctx.GetRegistry();
            
            // We need camera to project
            auto* cam = ctx.GetCamera();
            if (!cam) return false;
            
            glm::vec3 worldPos = cam->ScreenToWorld(m->position.x, m->position.y);

            // First click → store center point
            if (!m_Center.has_value()) {
                PositionComponent pc;
                pc.position = worldPos;
                Entity center = reg.CreateEntity();
                LOG(Info) << "[CircleTool] Added center at " << pc.position.x << " " << pc.position.y;
                reg.AddComponent(center, pc);

                m_Center = center;
                return true; // Consumed
            }

            // Second click → compute radius and create component
            Entity center = *m_Center;
            PositionComponent* pCenter = reg.GetComponent<PositionComponent>(center);
            if (!pCenter)
                return false;

            double dx = worldPos.x - pCenter->position.x;
            double dy = worldPos.y - pCenter->position.y;
            double radius = std::sqrt(dx * dx + dy * dy); // 2D distance on XY plane logic? 

            RadiusComponent rc;
            rc.radius = radius;
            reg.AddComponent(center, rc);

            LOG(Info) << "[CircleTool] Finalized circle r=" << radius;

            // DONE – reset for next circle
            m_Center.reset();
            return true; // Consumed
        }
    }
    
    return false;
}

}  // namespace nc::editor