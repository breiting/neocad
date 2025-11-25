#pragma once
#include <neocad/domain/IGeometryBackend.hpp>
#include <unordered_map>

// Forward declare OCCT types
class TopoDS_Shape;

namespace nc::occt {

/// OCCT-based geometry backend.
/// Owns a collection of TopoDS_Shapes referenced by opaque handles.
class OCCTBackend : public domain::IGeometryBackend {
   public:
    OCCTBackend();
    ~OCCTBackend() override;

    domain::BackendShapeHandle CreateExtrudedBody(const domain::Polygon& profile, double height) override;

    bool ExportShapeToSTEP(domain::BackendShapeHandle handle, const std::string& filePath) const override;
    bool ExportShapeToSTL(domain::BackendShapeHandle handle, const std::string& filePath,
                          double deflection) const override;

   private:
    domain::BackendShapeHandle m_NextHandle{1};  // start with 1
    std::unordered_map<domain::BackendShapeHandle, TopoDS_Shape*> m_Shapes;

    TopoDS_Shape* GetShape(domain::BackendShapeHandle handle) const;
    domain::BackendShapeHandle StoreShape(const TopoDS_Shape& shape);
};

}  // namespace nc::occt
