#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InputEvent.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <neocad/core/Logger.hpp>

using namespace nc::domain;

namespace nc::editor {

bool InsertPointTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (auto* m = AsMouseButton(ev)) {
        if (m->button == MouseButton::Left && m->pressed) {
            auto* cam = ctx.GetCamera();
            glm::vec3 pos = cam->ScreenToWorld(m->position.x, m->position.y);

            auto& reg = ctx.GetRegistry();
            Entity e = reg.CreateEntity();
            reg.AddComponent<PositionComponent>(e, {pos});
            reg.AddComponent<NameComponent>(e, {"Point"});

            // LOG(INFO) << "Inserted Point at " << pos.x << ", " << pos.y << ", " << pos.z;
            return true; // Consumed
        }
    }
    return false; // Not consumed
}

}  // namespace nc::editor
