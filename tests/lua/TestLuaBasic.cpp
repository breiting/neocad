#include <gtest/gtest.h>

#include <ontoflow/domain/Components.hpp>
#include <ontoflow/domain/GeometrySystem.hpp>
#include <ontoflow/domain/Registry.hpp>
#include <ontoflow/lua/CadAPI.hpp>
#include <ontoflow/lua/LuaEngine.hpp>
#include <ontoflow/occt/OCCTBackend.hpp>

using namespace of::domain;
using namespace of::occt;
using namespace of::lua;

TEST(LuaBasic, Simple) {
    Registry reg;
    OCCTBackend backend;
    GeometrySystem geom(reg, backend);

    CadAPI api(reg, geom);

    LuaEngine lua(api);
    std::string err;

    ASSERT_EQ(lua.Initialize(&err), true);

    bool res = lua.RunString(R"(
        p1 = cad.point(0, 0, 0)
        p2 = cad.point(5, 0, 0)
        p3 = cad.point(5, 10, 0)
        p4 = cad.point(0, 10, 0)
		l1 = cad.line(p1, p2)
		l2 = cad.line(p2, p3)
		l3 = cad.line(p3, p4)
		l4 = cad.line(p4, p1)
		f = cad.face({l1, l2, l3, l4})
		body = cad.extrude(f, 50)
		cad.save_step(body, "body.step")
		cad.save_stl(body, "body.stl")
    )");
    ASSERT_TRUE(res);
}
