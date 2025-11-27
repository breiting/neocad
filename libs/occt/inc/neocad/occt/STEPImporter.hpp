#pragma once
#include <neocad/domain/Registry.hpp>
#include <string>

class TopoDS_Shape;  // forward vom OCCT

namespace nc::occt {

class STEPImporter {
   public:
    STEPImporter() = default;

    /// @return Entity with MeshComponent or INVALID_ENTITY and modified registry
    nc::domain::Entity Load(const std::string& filename, nc::domain::Registry& registry);

   private:
    void TriangulateShape(const TopoDS_Shape& shape, nc::domain::MeshComponent& outMesh);
    void ExtractTopology(const TopoDS_Shape& shape, nc::domain::Registry& registry);
};

}  // namespace nc::occt
