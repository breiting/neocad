#include <imnodes.h>

#include <cctype>
#include <neocad/core/Logger.hpp>
#include <neocad/editor/Editor.hpp>
#include <neocad/ui/GraphEditorSystem.hpp>

namespace nc::editor {

/**
 * \brief Constructs an Editor instance.
 * \param ctx A reference to the ToolContext, providing access to core services.
 */
Editor::Editor(ToolContext& ctx) : m_Ctx(ctx) {
    m_Ctx.SetEditor(this);  // Set this Editor instance into the ToolContext

    // Initialize ImNodes
    ImNodes::CreateContext();
    ImNodes::StyleColorsDark();

    m_GraphEditorSystem = std::make_unique<ui::GraphEditorSystem>(ctx.GetRegistry());

    // Setup Demo Data
    auto& reg = ctx.GetRegistry();
    auto param = reg.CreateEntity();
    reg.AddComponent(param, domain::GlobalParameterComponent{"Width", 10.0, 1});
    reg.AddComponent(param, domain::UINodeComponent{50, 50});

    auto box = reg.CreateEntity();
    reg.AddComponent(box, domain::NameComponent{"MyBox"});
    reg.AddComponent(box, domain::BoxComponent{});
    reg.AddComponent(box, domain::UINodeComponent{250, 50});

    auto cyl = reg.CreateEntity();
    reg.AddComponent(cyl, domain::NameComponent{"MyCylinder"});
    reg.AddComponent(cyl, domain::CylinderComponent{});
    reg.AddComponent(cyl, domain::UINodeComponent{450, 50});
}

Editor::~Editor() {
    ImNodes::DestroyContext();
}

void Editor::DrawUI() {
    if (m_GraphEditorSystem) {
        m_GraphEditorSystem->DrawPanel();
    }
}

/**
 * \brief Registers a tool for a specific editor mode.
 * The editor takes ownership of the tool.
 * \param mode The EditorMode to associate the tool with.
 * \param tool A unique pointer to the ITool implementation.
 */
void Editor::RegisterTool(EditorMode mode, std::unique_ptr<ITool> tool) {
    m_Tools[mode] = std::move(tool);
}

/**
 * \brief Explicitly switches the editor to a new mode.
 * Informs the currently active tool (OnExit) and the new tool (OnEnter).
 *
 * \param mode The target EditorMode.
 */
void Editor::SetMode(EditorMode mode) {
    // Skip only if mode is same AND we have an active tool running
    if (m_Mode == mode && m_ActiveTool != nullptr)
        return;

    // inform old tool
    if (m_ActiveTool) {
        LOG(Info) << "Editor: Exiting tool for mode " << static_cast<int>(m_Mode);
        m_ActiveTool->OnExit(m_Ctx);
    }

    m_Mode = mode;
    auto it = m_Tools.find(m_Mode);
    m_ActiveTool = (it != m_Tools.end()) ? it->second.get() : nullptr;

    if (m_ActiveTool) {
        LOG(Info) << "Editor: Entering tool for mode " << static_cast<int>(m_Mode);
        m_ActiveTool->OnEnter(m_Ctx);
    } else {
        LOG(Warn) << "Editor: No tool registered for mode " << static_cast<int>(m_Mode);
    }

    // Clear command buffer on mode switch (Vim-like behavior)
    m_CommandBuffer.clear();
}

/**
 * \brief Called once per frame (or simulation step) to update active tools and camera.
 * \param dt Time delta since last frame.
 */
void Editor::Update(double dt) {
    if (m_ActiveTool)
        m_ActiveTool->Update(m_Ctx, dt);
    m_ViewController.Update(dt);
}

/**
 * \brief Sets the 2D camera for the ViewController.
 * \param cam A shared pointer to the 2D camera implementation.
 */
void Editor::SetCamera2D(std::shared_ptr<ICamera> cam) {
    m_ViewController.SetCamera2D(cam);
}

/**
 * \brief Sets the 3D camera for the ViewController.
 * \param cam A shared pointer to the 3D camera implementation.
 */
void Editor::SetCamera3D(std::shared_ptr<ICamera> cam) {
    m_ViewController.SetCamera3D(cam);
}

/**
 * \brief Sets the viewport size for the ViewController's cameras.
 * \param w Width of the viewport.
 * \param h Height of the viewport.
 */
void Editor::SetViewportSize(int w, int h) {
    m_ViewController.SetViewportSize(w, h);
}

/**
 * \brief Returns a pointer to the currently active camera.
 * \return A pointer to the ICamera.
 */
ICamera* Editor::GetActiveCamera() {
    return m_ViewController.GetActiveCamera();
}

/**
 * \brief Feeds an input event into the editor for processing.
 * Events are dispatched first to the active tool, then to the camera controller.
 * \param ev The input event.
 */
void Editor::OnInput(const InputEvent& ev) {
    // Global keyboard shortcuts
    if (auto* key = AsKey(ev)) {
        HandleKey(*key);
        // If a key event was handled globally (e.g., ESC, 'p'),
        // we might not want to pass it to the tool/camera.
        // For simplicity, let's assume HandleKey can change mode directly or is for non-tool-specific actions.
        // If HandleKey itself changes the mode, it implicitly takes precedence.
    }

    // 1. Send events to active tool FIRST
    bool handled = false;
    if (m_ActiveTool) {
        m_Ctx.SetCamera(m_ViewController.GetActiveCamera());  // Ensure tool has access to active camera
        handled = m_ActiveTool->OnInput(ev, m_Ctx);
    }

    // 2. If not handled by tool, pass to Camera Controller
    if (!handled) {
        m_ViewController.OnInput(ev);
    }
}

/**
 * \brief Handles keyboard events, including global shortcuts and command buffer input.
 * \param key The KeyEvent to process.
 */
void Editor::HandleKey(const KeyEvent& key) {
    if (!key.pressed)
        return;

    // ESCAPE key always returns to Normal Mode
    if (key.code == KeyCode::Escape) {
        LOG(Info) << "Editor: ESC pressed, switching to Normal mode.";
        SetMode(EditorMode::Normal);
        return;
    }

    // Collect commands only in Normal Mode (Vim-like behavior)
    if (m_Mode != EditorMode::Normal)
        return;

    // Ignore special keys (like Enter), tools should handle them if needed.
    if (key.text == 0)
        return;

    // Only process simple letter characters (without Ctrl/Alt modifiers)
    if (key.ctrl || key.alt)
        return;

    // Example: 'p' to dump registry (global shortcut)
    if (key.text == 'p') {
        LOG(Info) << "Editor: 'p' pressed, dumping registry...";
        m_Ctx.GetRegistry().Dump();
    }

    // 'g' to toggle graph editor
    if (key.text == 'g' && m_GraphEditorSystem) {
        LOG(Info) << "Editor: 'g' pressed, toggling graph editor...";
        m_GraphEditorSystem->ToggleVisibility();
    }

    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(key.text)));
    m_CommandBuffer.push_back(c);

    // Keep only the last 2 characters in the command buffer (for commands like "ip", "il", "ic", "is")
    if (m_CommandBuffer.size() > 2)
        m_CommandBuffer.erase(0,
                              m_CommandBuffer.size() - 2);  // Erase from beginning, keeping only the last 2 characters

    ProcessCommandBuffer();
}

/**
 * \brief Processes the current content of the command buffer.
 * Triggers mode switches based on recognized commands.
 */
void Editor::ProcessCommandBuffer() {
    if (m_CommandBuffer.size() < 2)
        return;  // Need at least 2 characters for known commands

    const std::string cmd = m_CommandBuffer;

    if (cmd == "ip") {
        LOG(Info) << "Editor: Command 'ip' received, switching to InsertPoint mode.";
        SetMode(EditorMode::InsertPoint);
    } else if (cmd == "il") {
        LOG(Info) << "Editor: Command 'il' received, switching to InsertLine mode.";
        SetMode(EditorMode::InsertLine);
    } else if (cmd == "ic") {
        LOG(Info) << "Editor: Command 'ic' received, switching to InsertCircle mode.";
        SetMode(EditorMode::InsertCircle);
    } else if (cmd == "is") {
        LOG(Info) << "Editor: Command 'is' received, switching to InsertSketch mode.";
        SetMode(EditorMode::InsertSketch);
    } else {
        // Unknown sequence, clear buffer and ignore
        LOG(Warn) << "Editor: Unknown command sequence '" << cmd << "', clearing command buffer.";
        m_CommandBuffer.clear();
        return;
    }

    // Clear buffer on successful command execution
    m_CommandBuffer.clear();
}

}  // namespace nc::editor
