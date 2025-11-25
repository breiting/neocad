#include <neocad/domain/Entity.hpp>
#include <neocad/lua/Bindings.hpp>

using namespace nc::domain;

namespace nc::lua {

void BindIO(sol::table& cad, CadAPI& api) {
    cad.set_function("save_step", [&api](Entity b, const std::string& path) {
        return api.ExportSTEP(b, path);
    });
    cad.set_function("save_stl", [&api](Entity b, const std::string& path) {
        return api.ExportSTL(b, path);
    });
}
}  // namespace nc::lua
