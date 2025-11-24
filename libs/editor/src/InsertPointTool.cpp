#include <neocad/domain/Components.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <variant>

namespace nc {

void InsertPointTool::OnInput(const InputEvent& event, ToolContext& ctx) {
    if (event.type != InputEventType::MouseButton)
        return;

    const auto* mb = std::get_if<MouseButtonEvent>(&event.data);
    if (!mb || !mb->pressed)
        return;

    // For now we assume (x,y) are already in world or sketch-plane coordinates.
    double wx = mb->x;
    double wy = mb->y;
    double wz = 0.0;

    auto& reg = ctx.GetRegistry();
    Entity e = reg.CreateEntity();
    PositionComponent pc;
    pc.position = vec3(wx, wy, wz);
    reg.AddComponent(e, pc);
}

}  // namespace nc
