#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI_Writer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <neocad/core/Logger.hpp>
#include <neocad/occt/OCCTBackend.hpp>

using namespace nc::domain;

namespace nc::occt {

OCCTBackend::OCCTBackend() = default;

OCCTBackend::~OCCTBackend() = default; // Unique_ptr handles cleanup

BackendShapeHandle OCCTBackend::StoreShape(const TopoDS_Shape& shape) {
    BackendShapeHandle handle = m_NextHandle++;
    m_Shapes[handle] = std::make_unique<TopoDS_Shape>(shape);
    return handle;
}

TopoDS_Shape* OCCTBackend::GetShape(BackendShapeHandle handle) const {
    auto it = m_Shapes.find(handle);
    if (it == m_Shapes.end()) {
        return nullptr;
    }
    return it->second.get();
}

BackendShapeHandle OCCTBackend::CreateExtrudedBody(const Polygon& profile, double height) {
    if (profile.vertices.size() < 3) {
        LOG(Error) << "CreateExtrudedBody: Profile has fewer than 3 vertices.";
        return 0;
    }

    BRepBuilderAPI_MakeWire wireBuilder;

    const auto& v = profile.vertices;
    const std::size_t n = v.size();
    for (std::size_t i = 0; i < n; ++i) {
        const auto& a = v[i];
        const auto& b = v[(i + 1) % n];
        BRepBuilderAPI_MakeEdge edge(gp_Pnt(a.x, a.y, a.z), gp_Pnt(b.x, b.y, b.z));
        if (!edge.IsDone()) {
             LOG(Error) << "CreateExtrudedBody: Failed to create edge " << i;
             return 0;
        }
        wireBuilder.Add(edge.Edge());
    }

    if (!wireBuilder.IsDone()) {
         LOG(Error) << "CreateExtrudedBody: Wire builder failed.";
         return 0;
    }

    TopoDS_Wire wire = wireBuilder.Wire();
    BRepBuilderAPI_MakeFace faceMaker(wire, true);
    if (!faceMaker.IsDone()) {
        LOG(Error) << "CreateExtrudedBody: Face maker failed.";
        return 0;
    }
    TopoDS_Face face = faceMaker.Face();

    gp_Vec dir(0.0, 0.0, height);
    BRepPrimAPI_MakePrism prism(face, dir);
    if (!prism.IsDone()) {
         LOG(Error) << "CreateExtrudedBody: Prism (extrusion) failed.";
         return 0;
    }
    TopoDS_Shape body = prism.Shape();

    return StoreShape(body);
}

bool OCCTBackend::ExportShapeToSTEP(BackendShapeHandle handle, const std::string& filePath) const {
    TopoDS_Shape* shape = GetShape(handle);
    if (!shape) {
        LOG(Error) << "ExportSTEP: Invalid handle " << handle;
        return false;
    }

    if (shape->IsNull()) {
        LOG(Error) << "ExportSTEP: Shape is null for handle " << handle;
        return false;
    }

    STEPControl_Writer writer;
    IFSelect_ReturnStatus status = writer.Transfer(*shape, STEPControl_AsIs);
    if (status != IFSelect_RetDone) {
        LOG(Error) << "ExportSTEP: Transfer failed with status " << (int)status;
        return false;
    }

    status = writer.Write(filePath.c_str());
    if (status != IFSelect_RetDone) {
        LOG(Error) << "ExportSTEP: Write failed with status " << (int)status;
        return false;
    }
    
    return true;
}

bool OCCTBackend::ExportShapeToSTL(BackendShapeHandle handle, const std::string& filePath, double deflection) const {
    TopoDS_Shape* shape = GetShape(handle);
    if (!shape) return false;

    if (shape->IsNull()) return false;

    BRepMesh_IncrementalMesh mesher(*shape, deflection);
    StlAPI_Writer stlWriter;
    stlWriter.Write(*shape, filePath.c_str());
    return true;
}

}  // namespace nc::occt
