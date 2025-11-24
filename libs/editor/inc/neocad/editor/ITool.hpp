#pragma once
#include <neocad/editor/InputEvent.hpp>

namespace nc {

class ToolContext;

/// Base interface for all editor tools.
/// Tools are mode-local state machines that react to input and mutate the ECS.
class ITool {
   public:
    virtual ~ITool() = default;

    /// Called when the tool becomes active (mode switch).
    virtual void OnEnter(ToolContext&) {
    }

    /// Called when the tool is deactivated.
    virtual void OnExit(ToolContext&) {
    }

    /// Called for every incoming input event while this tool is active.
    virtual void OnInput(const InputEvent& event, ToolContext& ctx) = 0;
};

}  // namespace nc
