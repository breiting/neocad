#pragma once
#include <memory>
#include <neocad/editor/ITool.hpp>
#include <neocad/editor/InputEvent.hpp>
#include <neocad/editor/Mode.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <string>
#include <unordered_map>

namespace nc::editor {

class Editor {
   public:
    explicit Editor(ToolContext& ctx);

    /// Called once per frame (or simulation step)
    void Update(double dt);

    /// Feed an input event into the editor (from UI layer).
    void OnInput(const InputEvent& ev);

    /// Register a tool for an editor mode.
    void RegisterTool(EditorMode mode, std::unique_ptr<ITool> tool);

    /// Explicit mode switch (can also be triggered by shortcuts).
    void SetMode(EditorMode mode);
    EditorMode GetMode() const {
        return m_Mode;
    }

    ToolContext& GetContext() {
        return m_Ctx;
    }
    const ToolContext& GetContext() const {
        return m_Ctx;
    }

   private:
    ToolContext& m_Ctx;
    EditorMode m_Mode{EditorMode::Normal};

    std::unordered_map<EditorMode, std::unique_ptr<ITool>> m_Tools;
    ITool* m_ActiveTool{nullptr};

    /// Command buffer for vim-like key sequences ("ip", "il", "ic", "if", ...)
    std::string m_CommandBuffer;

    void HandleKey(const KeyEvent& key);
    void ProcessCommandBuffer();
};

}  // namespace nc
