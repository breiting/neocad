#pragma once
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Types.hpp>
#include <string>

namespace nc::domain {

/// Interface for a geometry backend (OCCT or other).
class IGeometryBackend {
   public:
    virtual ~IGeometryBackend() = default;

    /// Create a solid body by extruding a planar polygon along its normal.
    virtual BackendShapeHandle CreateExtrudedBody(const Polygon& profile, double height) = 0;

    /// Export an existing backend shape as STEP.
    virtual bool ExportShapeToSTEP(BackendShapeHandle handle, const std::string& filePath) const = 0;

    /// Export as STL with a meshing tolerance.
    virtual bool ExportShapeToSTL(BackendShapeHandle handle, const std::string& filePath, double deflection) const = 0;
};

}  // namespace nc
