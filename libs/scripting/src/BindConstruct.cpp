#include <neocad/domain/Entity.hpp>
#include <neocad/scripting/Bindings.hpp>

namespace nc {

void BindConstruct(sol::table &cad, CadAPI &api) {
    cad.set_function("extrude", [&api](Entity f, double h) {
        return api.ExtrudeFace(f, h);
    });
}

}  // namespace nc
