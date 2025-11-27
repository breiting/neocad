#pragma once
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ICamera.hpp>

namespace nc::editor {

/// Lightweight façade passed into tools so they can operate on the ECS
/// and geometry services without knowing about UI or rendering.
class ToolContext {
   public:
    ToolContext(domain::Registry& registry, domain::GeometrySystem& geometrySystem)
        : m_Registry(registry), m_GeometrySystem(geometrySystem) {
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

    void SetCamera(ICamera* cam) {
        m_Camera = cam;
    }

    ICamera* GetCamera() const {
        return m_Camera;
    }

   private:
    domain::Registry& m_Registry;
    domain::GeometrySystem& m_GeometrySystem;
    ICamera* m_Camera;
};

}  // namespace nc::editor
