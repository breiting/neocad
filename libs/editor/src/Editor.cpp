#include <neocad/editor/Editor.hpp>

namespace nc {

Editor::Editor(ToolContext& ctx) : m_Ctx(ctx) {
}

void Editor::SetMode(EditorMode mode) {
    if (mode == m_Mode)
        return;

    // call OnExit on old tool if present
    auto itOld = m_Tools.find(m_Mode);
    if (itOld != m_Tools.end() && itOld->second) {
        itOld->second->OnExit(m_Ctx);
    }

    m_Mode = mode;

    // call OnEnter on new tool if present
    auto itNew = m_Tools.find(m_Mode);
    if (itNew != m_Tools.end() && itNew->second) {
        itNew->second->OnEnter(m_Ctx);
    }
}

void Editor::RegisterTool(EditorMode mode, std::unique_ptr<ITool> tool) {
    m_Tools[mode] = std::move(tool);
}

void Editor::OnInput(const InputEvent& event) {
    auto it = m_Tools.find(m_Mode);
    if (it == m_Tools.end() || !it->second)
        return;

    it->second->OnInput(event, m_Ctx);
}

}  // namespace nc
