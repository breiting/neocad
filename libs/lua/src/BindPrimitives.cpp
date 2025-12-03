#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/lua/Bindings.hpp>
#include <vector>

using namespace of::domain;

namespace of::lua {

void BindPrimitives(sol::table &cad, CadAPI &api) {
    cad.set_function("point", [&api](double x, double y, double z) {
        return api.CreatePoint(x, y, z);
    });
    cad.set_function("line", [&api](Entity a, Entity b) {
        return api.CreateLine(a, b);
    });
    cad.set_function("face", [&api](std::vector<Entity> pts) {
        return api.CreateFace(pts);
    });
}

}  // namespace of::lua
