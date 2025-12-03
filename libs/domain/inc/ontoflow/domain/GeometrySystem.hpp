#pragma once
#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/IGeometryBackend.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/domain/Types.hpp>

namespace of::domain {

/// High-level domain service responsible for:
/// - Extracting geometry from ECS (points, faces)
/// - Delegating to IGeometryBackend
/// - Writing resulting bodies back into ECS
class GeometrySystem {
   public:
    GeometrySystem(Registry& registry, IGeometryBackend& backend);

    // --- Creation ---
    Entity CreatePoint(const vec3& pos);
    Entity CreateLine(Entity p1, Entity p2);
    Entity CreateFace(const std::vector<Entity>& edges);

    // --- Modification ---
    Entity ExtrudeFace(Entity face, double height);

    // --- Export ---
    bool ExportSTEP(Entity body, const std::string& path) const;
    bool ExportSTL(Entity body, const std::string& path, double deflection) const;

   private:
    Registry& m_Registry;
    IGeometryBackend& m_Backend;

    /// Build polygon from a FaceComponent/LineComponents/PointComponents.
    bool BuildPolygonFromFace(Entity faceEntity, Polygon& outPolygon) const;
};

}  // namespace of::domain
