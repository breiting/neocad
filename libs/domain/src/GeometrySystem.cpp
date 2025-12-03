#include <ontoflow/domain/GeometrySystem.hpp>

#include <ontoflow/core/Logger.hpp>
#include "ontoflow/domain/Components.hpp"
#include "ontoflow/domain/Entity.hpp"

namespace of::domain {

GeometrySystem::GeometrySystem(Registry& reg, IGeometryBackend& backend) : m_Registry(reg), m_Backend(backend) {
}

// Methods commented out during refactor

}  // namespace of::domain