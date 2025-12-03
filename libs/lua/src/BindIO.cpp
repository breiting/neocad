#include <ontoflow/domain/Entity.hpp>
#include <ontoflow/lua/Bindings.hpp>

using namespace of::domain;

namespace of::lua {

void BindIO(sol::table& cad, CadAPI& api) {
    cad.set_function("save_step", [&api](Entity b, const std::string& path) {
        return api.ExportSTEP(b, path);
    });
    cad.set_function("save_stl", [&api](Entity b, const std::string& path) {
        return api.ExportSTL(b, path);
    });
}
}  // namespace of::lua
