#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/editor/InputEvent.hpp>
#include <ontoflow/editor/InsertPointTool.hpp>
#include <ontoflow/editor/ToolContext.hpp>
#include <ontoflow/core/Logger.hpp>

using namespace of::domain;

namespace of::editor {

/**
 * \brief Handles input events for the tool.
 * Processes left mouse clicks to insert a point.
 * \param ev The input event to process.
 * \param ctx The ToolContext for accessing core services.
 * \return True if the event was consumed by the tool, false otherwise.
 */
bool InsertPointTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            auto* cam = ctx.GetCamera();
            if (!cam) {
                LOG(Error) << "InsertPointTool: No active camera for screen-to-world conversion.";
                return false; // Event not handled, no mode change
            }

            glm::vec3 pos = cam->ScreenToWorld(m->position.x, m->position.y);

            auto& reg = ctx.GetRegistry();
            Entity e = reg.CreateEntity();
            reg.AddComponent<PositionComponent>(e, {pos});
            reg.AddComponent<NameComponent>(e, {"Point"});

            LOG(Info) << "InsertPointTool: Inserted Point " << e << " at (" << pos.x << ", " << pos.y << ", " << pos.z << ")";
            return true; // Event handled, no mode change
        }
    }
    return false; // Event not handled by this tool, no mode change
}

}  // namespace of::editor
