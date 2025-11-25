#pragma once
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/Registry.hpp>

namespace nc::editor {

/// Lightweight façade passed into tools so they can operate on the ECS
/// and geometry services without knowing about UI or rendering.
class ToolContext {
   public:
    ToolContext(Registry& registry, GeometrySystem& geometrySystem)
        : m_Registry(registry), m_GeometrySystem(geometrySystem) {
    }

    Registry& GetRegistry() {
        return m_Registry;
    }
    const Registry& GetRegistry() const {
        return m_Registry;
    }

    GeometrySystem& GetGeometrySystem() {
        return m_GeometrySystem;
    }
    const GeometrySystem& GetGeometrySystem() const {
        return m_GeometrySystem;
    }

   private:
    Registry& m_Registry;
    GeometrySystem& m_GeometrySystem;
};

}  // namespace nc
