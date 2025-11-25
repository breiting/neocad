#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/ToolContext.hpp>

namespace nc::editor {

void InsertPointTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (auto* m = AsMouseButton(ev)) {
        if (!(m->button == MouseButton::Left))
            return;
        double wx = m->position.x;
        double wy = m->position.y;
        double wz = 0.0;

        auto& reg = ctx.GetRegistry();
        Entity e = reg.CreateEntity();
        PositionComponent pc;
        pc.position = vec3(wx, wy, wz);
        reg.AddComponent(e, pc);
    }
}

}  // namespace nc
