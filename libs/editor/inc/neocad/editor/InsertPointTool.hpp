#pragma once
#include <neocad/editor/ITool.hpp>

namespace nc::editor {

/// Simple tool: on mouse click, creates a point entity in the ECS.
class InsertPointTool : public ITool {
   public:
    void OnEnter(ToolContext&) override {
    }
    void OnExit(ToolContext&) override {
    }

    void OnInput(const InputEvent& event, ToolContext& ctx) override;

    void Update(ToolContext& /*m_Ctx*/, double /*dt*/) override {
    }
};

}  // namespace nc::editor
