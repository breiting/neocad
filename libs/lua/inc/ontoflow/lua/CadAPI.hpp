#pragma once
#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/domain/GeometrySystem.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <string>
#include <vector>

namespace of::lua {

class CadAPI {
   public:
    CadAPI(domain::Registry&, domain::GeometrySystem&);

    domain::Entity CreatePoint(double x, double y, double z);
    domain::Entity CreateLine(domain::Entity p1, domain::Entity p2);
    domain::Entity CreateFace(const std::vector<domain::Entity>& pts);
    domain::Entity ExtrudeFace(domain::Entity face, double h);
    bool ExportSTEP(domain::Entity body, const std::string& filename);
    bool ExportSTL(domain::Entity body, const std::string& filename, double deflection = 0.1);

   private:
    domain::Registry& m_Registry;
    domain::GeometrySystem& m_GeometrySystem;
};

}  // namespace of::lua
