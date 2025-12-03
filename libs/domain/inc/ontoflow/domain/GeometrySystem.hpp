#pragma once
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/IGeometryBackend.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/domain/Types.hpp>

namespace of::domain {

/// High-level domain service.
/// Legacy constructive geometry methods have been removed.
class GeometrySystem {
   public:
    GeometrySystem(Registry& registry, IGeometryBackend& backend);

    IGeometryBackend& GetBackend() { return m_Backend; }

    // Methods removed during refactor.
    // ...

   private:
    Registry& m_Registry;
    IGeometryBackend& m_Backend;
};

}  // namespace of::domain
