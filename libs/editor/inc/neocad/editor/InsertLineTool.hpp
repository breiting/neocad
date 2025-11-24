#pragma once
#include <neocad/domain/Entity.hpp>
#include <neocad/editor/ITool.hpp>
#include <optional>

namespace nc {

/// Tool for creating a polyline segment (simple 2-point line MVP).
/// First click: create start point.
/// Second click: create end point + line entity connecting them.
class InsertLineTool : public ITool {
   public:
    void OnEnter(ToolContext&) override;
    void OnExit(ToolContext&) override;

    void OnInput(const InputEvent& event, ToolContext& ctx) override;

   private:
    std::optional<Entity> m_PendingStartPoint;
};

}  // namespace nc
