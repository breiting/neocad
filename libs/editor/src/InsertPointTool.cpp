#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/ToolContext.hpp>

using namespace nc::domain;

namespace nc::editor {

void InsertPointTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (auto* m = AsMouseButton(ev)) {
        if (m->button != MouseButton::Left)
            return;

        if (!m->pressed)
            return;

        ICamera* cam = ctx.GetCamera();
        if (!cam)
            return;

        glm::vec3 world = cam->ScreenToWorld(m->position.x, m->position.y);
        printf("%f %f %f\n", world.x, world.y, world.z);

        auto& reg = ctx.GetRegistry();
        Entity e = reg.CreateEntity();
        PositionComponent pc;
        pc.position = world;
        reg.AddComponent(e, pc);
    }
}

}  // namespace nc::editor
