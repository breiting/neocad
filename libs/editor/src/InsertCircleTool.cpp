#include <glm/glm.hpp>
#include <ontoflow/core/Logger.hpp>
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/editor/InputEvent.hpp>
#include <ontoflow/editor/InsertCircleTool.hpp>
#include <ontoflow/editor/ToolContext.hpp>

using namespace of::domain;

namespace of::editor {

/**
 * \brief Called when the tool becomes active.
 * Resets any internal state, such as pending center point.
 * \param ctx The ToolContext for accessing core services.
 */
void InsertCircleTool::OnEnter(ToolContext& /*ctx*/) {
    // Optionally reset state
    m_Center.reset();
    LOG(Info) << "InsertCircleTool: Entered.";
}

/**
 * \brief Called when the tool becomes inactive.
 * Cleans up any pending state or temporary entities.
 * \param ctx The ToolContext for accessing core services.
 */
void InsertCircleTool::OnExit(ToolContext& ctx) {
    // Cleanup if mid-operation? If m_Center has a value, it means a circle was partially defined.
    // The point entity for the center should be removed from the registry.
    if (m_Center.has_value()) {
        auto& reg = ctx.GetRegistry();
        // Remove PositionComponent and RadiusComponent from the entity
        reg.RemoveComponent<PositionComponent>(*m_Center);
        reg.RemoveComponent<RadiusComponent>(*m_Center);
        // Note: The entity ID itself remains, but with no components it's effectively "empty".
        LOG(Info) << "InsertCircleTool: Exited, cleaning up pending center point " << *m_Center;
    }
    m_Center.reset();
    LOG(Info) << "InsertCircleTool: Exited.";
}

/**
 * \brief Handles input events for the tool.
 * Processes mouse clicks to define the circle's center and radius.
 * \param ev The input event to process.
 * \param ctx The ToolContext for accessing core services.
 * \return True if the event was consumed by the tool, false otherwise.
 */
bool InsertCircleTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            auto& reg = ctx.GetRegistry();
            
            // We need camera to project
            auto* cam = ctx.GetCamera();
            if (!cam) {
                LOG(Error) << "InsertCircleTool: No active camera for screen-to-world conversion.";
                return false; // Event not handled, no mode change
            }
            
            glm::vec3 worldPos = cam->ScreenToWorld(m->position.x, m->position.y);

            // First click → store center point
            if (!m_Center.has_value()) {
                PositionComponent pc;
                pc.position = worldPos;
                Entity center = reg.CreateEntity();
                LOG(Info) << "[InsertCircleTool] Added center point " << center << " at " << pc.position.x << ", " << pc.position.y;
                reg.AddComponent(center, pc);
                reg.AddComponent<NameComponent>(center, {"CircleCenter"}); // Add name for debugging

                m_Center = center;
                return true; // Event handled, no mode change
            }

            // Second click → compute radius and create component
            Entity centerEntity = *m_Center;
            PositionComponent* pCenter = reg.GetComponent<PositionComponent>(centerEntity);
            if (!pCenter) {
                LOG(Error) << "InsertCircleTool: Center point entity " << centerEntity << " has no PositionComponent.";
                m_Center.reset(); // Reset tool if state is invalid
                return false; // Event not handled, no mode change
            }

            double dx = worldPos.x - pCenter->position.x;
            double dy = worldPos.y - pCenter->position.y;
            double radius = std::sqrt(dx * dx + dy * dy); // 2D distance on XY plane logic? 

            if (radius < 1e-6) { // Prevent creating tiny/degenerate circles
                LOG(Warn) << "InsertCircleTool: Radius too small, ignoring second click.";
                return true; // Consumed but no circle created
            }

            RadiusComponent rc;
            rc.radius = radius;
            // Add or update RadiusComponent for the center entity
            reg.AddComponent(centerEntity, rc);

            LOG(Info) << "[InsertCircleTool] Finalized circle " << centerEntity << " with radius=" << radius;

            // DONE – reset for next circle
            m_Center.reset();
            return true; // Event handled, no mode change
        }
    }
    
    return false; // Event not handled by this tool, no mode change
}

}  // namespace of::editor