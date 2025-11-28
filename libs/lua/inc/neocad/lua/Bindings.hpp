#pragma once
#include <neocad/lua/CadAPI.hpp>
#include <sol/sol.hpp>

namespace nc::lua {

void BindPrimitives(sol::table& cad, CadAPI& api);
void BindConstruct(sol::table& cad, CadAPI& api);
void BindIO(sol::table& cad, CadAPI& api);
}  // namespace nc::lua
