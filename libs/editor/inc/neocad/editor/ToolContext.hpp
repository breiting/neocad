#pragma once
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>

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

   private:
    domain::Registry& m_Registry;
    domain::GeometrySystem& m_GeometrySystem;
};

}  // namespace nc::editor
