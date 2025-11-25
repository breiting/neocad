#pragma once
#include <neocad/domain/Entity.hpp>
#include <string>
#include <vector>

namespace nc::lua {

class Registry;
class GeometrySystem;

class CadAPI {
   public:
    CadAPI(Registry&, GeometrySystem&);

    Entity CreatePoint(double x, double y, double z);
    Entity CreateLine(Entity p1, Entity p2);
    Entity CreateFace(const std::vector<Entity>& pts);
    Entity ExtrudeFace(Entity face, double h);
    bool ExportSTEP(Entity body, const std::string& filename);
    bool ExportSTL(Entity body, const std::string& filename, double deflection = 0.1);

   private:
    Registry& m_Registry;
    GeometrySystem& m_GeometrySystem;
};

}  // namespace nc
