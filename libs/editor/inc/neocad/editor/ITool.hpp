#pragma once
#include <neocad/editor/InputEvent.hpp>

namespace nc::editor {

class ToolContext;

/// Base interface for all editor tools.
/// Tools are mode-local state machines that react to input and mutate the ECS.
class ITool {
   public:
    virtual ~ITool() = default;

    /// Called when the tool becomes active (mode switch).
    virtual void OnEnter(ToolContext&) {
    }

    virtual void OnExit(ToolContext& ctx) = 0;

    /// Handle input event. Return true if consumed.
    virtual bool OnInput(const InputEvent& ev, ToolContext& ctx) = 0;

    virtual void Update(ToolContext& /*ctx*/, double /*dt*/) {}
};

}  // namespace nc::editor
