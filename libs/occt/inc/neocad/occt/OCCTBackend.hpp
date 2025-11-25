#pragma once
#include <neocad/domain/IGeometryBackend.hpp>
#include <unordered_map>

// Forward declare OCCT types
class TopoDS_Shape;

namespace nc::occt {

/// OCCT-based geometry backend.
/// Owns a collection of TopoDS_Shapes referenced by opaque handles.
class OCCTBackend : public IGeometryBackend {
   public:
    OCCTBackend();
    ~OCCTBackend() override;

    BackendShapeHandle CreateExtrudedBody(const Polygon& profile, double height) override;

    bool ExportShapeToSTEP(BackendShapeHandle handle, const std::string& filePath) const override;
    bool ExportShapeToSTL(BackendShapeHandle handle, const std::string& filePath, double deflection) const override;

   private:
    BackendShapeHandle m_NextHandle{1};  // start with 1
    std::unordered_map<BackendShapeHandle, TopoDS_Shape*> m_Shapes;

    TopoDS_Shape* GetShape(BackendShapeHandle handle) const;
    BackendShapeHandle StoreShape(const TopoDS_Shape& shape);
};

}  // namespace nc
