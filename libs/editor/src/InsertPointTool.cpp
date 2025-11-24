#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/ToolContext.hpp>

namespace nc {

void InsertPointTool::OnInput(const InputEvent& ev, ToolContext& ctx) {
    if (ev.IsLeftMouseClick()) {
        const auto& m = ev.AsMouseButton();
        double wx = m.x;
        double wy = m.y;
        double wz = 0.0;

        auto& reg = ctx.GetRegistry();
        Entity e = reg.CreateEntity();
        PositionComponent pc;
        pc.position = vec3(wx, wy, wz);
        reg.AddComponent(e, pc);
    }
}

}  // namespace nc
