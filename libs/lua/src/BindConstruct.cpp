#include <neocad/domain/Entity.hpp>
#include <neocad/lua/Bindings.hpp>

using namespace nc::domain;

namespace nc::lua {

void BindConstruct(sol::table &cad, CadAPI &api) {
    cad.set_function("extrude", [&api](Entity f, double h) {
        return api.ExtrudeFace(f, h);
    });
}

}  // namespace nc::lua
