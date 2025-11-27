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

    bool OnInput(const InputEvent& ev, ToolContext& ctx) override;
};

}  // namespace nc::editor
