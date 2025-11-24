#pragma once
#include <memory>
#include <neocad/editor/ITool.hpp>
#include <neocad/editor/InputEvent.hpp>
#include <neocad/editor/Mode.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <unordered_map>

namespace nc {

/// Central editor controller.
/// - Holds current mode.
/// - Dispatches input events to the active tool.
/// - Owns tools mapped to modes.
class Editor {
   public:
    explicit Editor(ToolContext& ctx);

    /// Set the current mode, switching tools accordingly.
    void SetMode(EditorMode mode);

    /// Get current mode.
    EditorMode GetMode() const {
        return m_Mode;
    }

    /// Register a tool for a given mode.
    void RegisterTool(EditorMode mode, std::unique_ptr<ITool> tool);

    /// Process one input event (key, mouse, ...).
    void OnInput(const InputEvent& event);

   private:
    ToolContext& m_Ctx;
    EditorMode m_Mode{EditorMode::Normal};
    std::unordered_map<EditorMode, std::unique_ptr<ITool>> m_Tools;
};

}  // namespace nc
