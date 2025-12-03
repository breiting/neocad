#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/lua/Bindings.hpp>

using namespace of::domain;

namespace of::lua {

void BindConstruct(sol::table &cad, CadAPI &api) {
    cad.set_function("extrude", [&api](Entity f, double h) {
        return api.ExtrudeFace(f, h);
    });
}

}  // namespace of::lua
