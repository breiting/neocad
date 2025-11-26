#include <cctype>
#include <neocad/editor/Editor.hpp>

namespace nc::editor {

Editor::Editor(ToolContext& ctx) : m_Ctx(ctx) {
}

void Editor::RegisterTool(EditorMode mode, std::unique_ptr<ITool> tool) {
    m_Tools[mode] = std::move(tool);
}

void Editor::SetMode(EditorMode mode) {
    if (m_Mode == mode)
        return;

    // inform old tool
    if (m_ActiveTool)
        m_ActiveTool->OnExit(m_Ctx);

    m_Mode = mode;
    auto it = m_Tools.find(m_Mode);
    m_ActiveTool = (it != m_Tools.end()) ? it->second.get() : nullptr;

    if (m_ActiveTool)
        m_ActiveTool->OnEnter(m_Ctx);

    // beim Moduswechsel: Command-Buffer leeren (vim-like)
    m_CommandBuffer.clear();
}

void Editor::Update(double dt) {
    if (m_ActiveTool)
        m_ActiveTool->Update(m_Ctx, dt);
}

void Editor::SetCamera2D(std::shared_ptr<ICamera> cam) {
    m_ViewController.SetCamera2D(cam);
}

void Editor::SetCamera3D(std::shared_ptr<ICamera> cam) {
    m_ViewController.SetCamera3D(cam);
}

void Editor::SetViewportSize(int w, int h) {
    m_ViewController.SetViewportSize(w, h);
}

ICamera* Editor::GetActiveCamera() {
    return m_ViewController.GetActiveCamera();
}

void Editor::OnInput(const InputEvent& ev) {
    m_ViewController.OnInput(ev);

    // Global keyboard shortcuts
    if (auto* key = AsKey(ev)) {
        HandleKey(*key);
    }

    // Send events to active tool
    if (m_ActiveTool) {
        m_ActiveTool->OnInput(ev, m_Ctx);
    }
}

void Editor::HandleKey(const KeyEvent& key) {
    printf("%c %d\n", key.text, key.pressed);
    if (!key.pressed)
        return;

    // ESC → immer in Normal-Mode zurück
    if (key.code == KeyCode::Escape) {
        SetMode(EditorMode::Normal);
        return;
    }

    // Nur im Normal-Mode Kommandos sammeln (wie in Vim)
    if (m_Mode != EditorMode::Normal)
        return;

    // Sondertasten wie Enter hier ignorieren, Tools fangen die selbst
    if (key.text == 0)
        return;

    // Nur einfache Buchstaben (ohne Ctrl/Alt) berücksichtigen
    if (key.ctrl || key.alt)
        return;

    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(key.text)));
    m_CommandBuffer.push_back(c);

    // Wir brauchen nur die letzten 2 Zeichen (für "ip","il","ic","if")
    if (m_CommandBuffer.size() > 2)
        m_CommandBuffer.erase(m_CommandBuffer.begin(), m_CommandBuffer.end() - 2);

    ProcessCommandBuffer();
}

// ---------------------------------------------------------------------
void Editor::ProcessCommandBuffer() {
    if (m_CommandBuffer.size() < 2)
        return;

    const std::string cmd = m_CommandBuffer;

    if (cmd == "ip") {
        SetMode(EditorMode::InsertPoint);
    } else if (cmd == "il") {
        SetMode(EditorMode::InsertLine);
    } else if (cmd == "ic") {
        SetMode(EditorMode::InsertCircle);
    } else if (cmd == "is") {
        SetMode(EditorMode::InsertSketch);
    } else {
        // unknown sequence
        return;
    }

    // clear buffer on success
    m_CommandBuffer.clear();
}

}  // namespace nc::editor
