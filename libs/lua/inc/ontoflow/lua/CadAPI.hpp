#pragma once
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/domain/GeometrySystem.hpp>

namespace of::lua {

class CadAPI {
   public:
    CadAPI(domain::Registry&, domain::GeometrySystem&);

   private:
    domain::Registry& m_Registry;
    domain::GeometrySystem& m_GeometrySystem;
};

}  // namespace of::lua