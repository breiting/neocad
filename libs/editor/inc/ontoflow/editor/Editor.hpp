#pragma once
#include <memory>
#include <ontoflow/editor/ITool.hpp>
#include <ontoflow/editor/InputEvent.hpp>
#include <ontoflow/editor/Mode.hpp>
#include <ontoflow/editor/ToolContext.hpp>
#include <string>
#include <unordered_map>

#include "ontoflow/editor/ViewController.hpp" // Fully includes ViewController, instead of just forward declaring if used.

// Forward declaration
namespace of::ui { class GraphEditorSystem; }

namespace of::editor {

/**
 * \brief The central editor class managing modes, tools, and input dispatch.
 *
 * The Editor orchestrates user interaction by maintaining a current `EditorMode`
 * and dispatching input events to the active `ITool`. It also manages the
 * `ViewController` for camera control and processes a Vim-like command buffer.
 */
class Editor {
   public:
    /**
     * \brief Constructs an Editor instance.
     * \param ctx A reference to the ToolContext, providing access to core services.
     */
    explicit Editor(ToolContext& ctx);
    ~Editor(); // Destructor needed for unique_ptr forward declaration

    /**
     * \brief Called once per frame (or simulation step) to update active tools and camera.
     * \param dt Time delta since last frame.
     */
    void Update(double dt);

    /**
     * \brief Draws the UI panels.
     */
    void DrawUI();

    /**
     * \brief Feeds an input event into the editor for processing.
     * Events are dispatched first to the active tool, then to the camera controller.
     * \param ev The input event.
     */
    void OnInput(const InputEvent& ev);

    /**
     * \brief Registers a tool for a specific editor mode.
     * The editor takes ownership of the tool.
     * \param mode The EditorMode to associate the tool with.
     * \param tool A unique pointer to the ITool implementation.
     */
    void RegisterTool(EditorMode mode, std::unique_ptr<ITool> tool);

    /**
     * \brief Explicitly switches the editor to a new mode.
     * Informs the currently active tool (OnExit) and the new tool (OnEnter).
     * \param mode The target EditorMode.
     */
    void SetMode(EditorMode mode);

    /**
     * \brief Returns the current active editor mode.
     * \return The current EditorMode.
     */
    EditorMode GetMode() const {
        return m_Mode;
    }

    /**
     * \brief Returns a mutable reference to the editor's ToolContext.
     * \return A reference to the ToolContext.
     */
    ToolContext& GetContext() {
        return m_Ctx;
    }

    /**
     * \brief Returns a constant reference to the editor's ToolContext.
     * \return A const reference to the ToolContext.
     */
    const ToolContext& GetContext() const {
        return m_Ctx;
    }

    // Adapter for ViewController
    /**
     * \brief Sets the 2D camera for the ViewController.
     * \param cam A shared pointer to the 2D camera implementation.
     */
    void SetCamera2D(std::shared_ptr<ICamera> cam);

    /**
     * \brief Sets the 3D camera for the ViewController.
     * \param cam A shared pointer to the 3D camera implementation.
     */
    void SetCamera3D(std::shared_ptr<ICamera> cam);

    /**
     * \brief Sets the viewport size for the ViewController's cameras.
     * \param w Width of the viewport.
     * \param h Height of the viewport.
     */
    void SetViewportSize(int w, int h);

    /**
     * \brief Returns a pointer to the currently active camera.
     * \return A pointer to the ICamera.
     */
    ICamera* GetActiveCamera();

   private:
    ToolContext& m_Ctx; ///< The tool context providing access to core services.
    ViewController m_ViewController; ///< Manages camera views and input dispatch.
    EditorMode m_Mode{EditorMode::Normal}; ///< The current active editor mode.

    std::unordered_map<EditorMode, std::unique_ptr<ITool>> m_Tools; ///< Registered tools, keyed by EditorMode.
    ITool* m_ActiveTool{nullptr}; ///< Pointer to the currently active tool.

    std::string m_CommandBuffer; ///< Buffer for Vim-like key sequences.
    
    std::unique_ptr<ui::GraphEditorSystem> m_GraphEditorSystem; ///< The graph editor system.

    /**
     * \brief Handles keyboard events, including global shortcuts and command buffer input.
     * \param key The KeyEvent to process.
     */
    void HandleKey(const KeyEvent& key);

    /**
     * \brief Processes the current content of the command buffer.
     * Triggers mode switches based on recognized commands.
     */
    void ProcessCommandBuffer();
};

}  // namespace of::editor
