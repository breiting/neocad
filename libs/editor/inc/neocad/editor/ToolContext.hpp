#pragma once
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ICamera.hpp>
#include <neocad/command/CommandStack.hpp>

namespace nc::editor {

class Editor; // Forward declaration

/**
 * \brief Lightweight facade passed into tools so they can operate on the ECS,
 * geometry services, and command stack without direct knowledge of UI or rendering.
 *
 * This class provides a controlled access point for tools to interact with
 * core application services.
 */
class ToolContext {
   public:
    /**
     * \brief Constructs a ToolContext.
     * \param registry Reference to the central ECS registry.
     * \param geometrySystem Reference to the geometry system.
     * \param cmdStack Reference to the command stack for undo/redo.
     */
    ToolContext(domain::Registry& registry, domain::GeometrySystem& geometrySystem, cmd::CommandStack& cmdStack)
        : m_Registry(registry), m_GeometrySystem(geometrySystem), m_CommandStack(cmdStack) {
    }

    domain::Registry& GetRegistry() {
        return m_Registry;
    }
    const domain::Registry& GetRegistry() const {
        return m_Registry;
    }

    domain::GeometrySystem& GetGeometrySystem() {
        return m_GeometrySystem;
    }
    const domain::GeometrySystem& GetGeometrySystem() const {
        return m_GeometrySystem;
    }

    cmd::CommandStack& GetCommandStack() {
        return m_CommandStack;
    }

    void SetCamera(ICamera* cam) {
        m_Camera = cam;
    }

    ICamera* GetCamera() const {
        return m_Camera;
    }

    /**
     * \brief Sets the Editor instance for the context.
     * This allows tools to request mode changes or other Editor-specific actions.
     * \param editor A pointer to the Editor instance.
     */
    void SetEditor(Editor* editor) {
        m_Editor = editor;
    }

    /**
     * \brief Returns a pointer to the Editor instance.
     * \return A pointer to the Editor, or nullptr if not set.
     */
    Editor* GetEditor() const {
        return m_Editor;
    }

   private:
    domain::Registry& m_Registry; ///< Reference to the ECS registry.
    domain::GeometrySystem& m_GeometrySystem; ///< Reference to the geometry system.
    cmd::CommandStack& m_CommandStack; ///< Reference to the command stack.
    ICamera* m_Camera = nullptr; ///< Pointer to the currently active camera.
    Editor* m_Editor = nullptr; ///< Pointer to the Editor instance.
};

}  // namespace nc::editor
