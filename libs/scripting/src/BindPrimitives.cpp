#include <neocad/domain/Entity.hpp>
#include <neocad/scripting/Bindings.hpp>
#include <vector>

namespace nc {

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

}  // namespace nc
