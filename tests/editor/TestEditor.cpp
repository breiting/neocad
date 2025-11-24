#include <gtest/gtest.h>

#include <neocad/domain/Components.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/Editor.hpp>
#include <neocad/editor/InsertLineTool.hpp>
#include <neocad/editor/InsertPointTool.hpp>

using namespace nc;

// --- Minimal mock backend for GeometrySystem --------------------------------
class DummyBackend : public IGeometryBackend {
   public:
    BackendShapeHandle CreateExtrudedBody(const Polygon& profile, double height) override {
        lastProfile = profile;
        lastHeight = height;
        return ++lastHandle;
    }

    bool ExportShapeToSTEP(BackendShapeHandle, const std::string&) const override {
        return true;
    }

    bool ExportShapeToSTL(BackendShapeHandle, const std::string&, double) const override {
        return true;
    }

    Polygon lastProfile;
    double lastHeight{0.0};
    BackendShapeHandle lastHandle{0};
};

// --- Helper: count components -----------------------------------------------

static size_t CountPoints(const Registry& reg) {
    size_t count = 0;
    for (Entity e : reg.Entities()) {
        if (reg.HasComponent<PositionComponent>(e))
            ++count;
    }
    return count;
}

static size_t CountLines(const Registry& reg) {
    size_t count = 0;
    for (Entity e : reg.Entities()) {
        if (reg.HasComponent<LineComponent>(e))
            ++count;
    }
    return count;
}

// --- Tests ------------------------------------------------------------------

// 1) Editor starts in Normal mode
TEST(EditorBasics, StartsInNormalMode) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    ToolContext ctx(reg, geom);

    Editor editor(ctx);
    EXPECT_EQ(editor.GetMode(), EditorMode::Normal);
}

// 2) Mode switching triggers OnEnter/OnExit
class TestTool : public ITool {
   public:
    int enterCount = 0;
    int exitCount = 0;
    int inputCount = 0;

    void OnEnter(ToolContext&) override {
        ++enterCount;
    }
    void OnExit(ToolContext&) override {
        ++exitCount;
    }
    void OnInput(const InputEvent&, ToolContext&) override {
        ++inputCount;
    }
};

TEST(EditorBasics, ModeSwitchCallsEnterExit) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    ToolContext ctx(reg, geom);

    Editor editor(ctx);

    auto toolNormal = std::make_unique<TestTool>();
    auto* ptrNormal = toolNormal.get();
    editor.RegisterTool(EditorMode::Normal, std::move(toolNormal));

    auto toolInsert = std::make_unique<TestTool>();
    auto* ptrInsert = toolInsert.get();
    editor.RegisterTool(EditorMode::InsertPoint, std::move(toolInsert));

    // Initially in Normal, so Normal's OnEnter should have been called once
    editor.SetMode(EditorMode::Normal);
    EXPECT_EQ(ptrNormal->enterCount, 1);
    EXPECT_EQ(ptrNormal->exitCount, 0);

    // Switch to InsertPoint
    editor.SetMode(EditorMode::InsertPoint);
    EXPECT_EQ(ptrNormal->exitCount, 1);
    EXPECT_EQ(ptrInsert->enterCount, 1);

    // Switch back to Normal
    editor.SetMode(EditorMode::Normal);
    EXPECT_EQ(ptrInsert->exitCount, 1);
    EXPECT_EQ(ptrNormal->enterCount, 2);  // entered again
}

// 3) InsertPointTool creates one point per click
TEST(InsertPointToolTests, CreatesPointOnClick) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    ToolContext ctx(reg, geom);

    Editor editor(ctx);
    editor.RegisterTool(EditorMode::InsertPoint, std::make_unique<InsertPointTool>());
    editor.SetMode(EditorMode::InsertPoint);

    InputEvent ev;
    ev.type = InputEventType::MouseButton;
    MouseButtonEvent mb;
    mb.button = 0;
    mb.pressed = true;
    mb.x = 10.0;
    mb.y = 20.0;
    ev.data = mb;

    editor.OnInput(ev);

    EXPECT_EQ(CountPoints(reg), 1u);
}

// 4) InsertLineTool creates 2 points and 1 line after 2 clicks
TEST(InsertLineToolTests, CreatesLineAfterTwoClicks) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    ToolContext ctx(reg, geom);

    Editor editor(ctx);
    editor.RegisterTool(EditorMode::InsertLine, std::make_unique<InsertLineTool>());
    editor.SetMode(EditorMode::InsertLine);

    InputEvent ev1;
    ev1.type = InputEventType::MouseButton;
    MouseButtonEvent mb1;
    mb1.button = 0;
    mb1.pressed = true;
    mb1.x = 0.0;
    mb1.y = 0.0;
    ev1.data = mb1;

    InputEvent ev2;
    ev2.type = InputEventType::MouseButton;
    MouseButtonEvent mb2;
    mb2.button = 0;
    mb2.pressed = true;
    mb2.x = 10.0;
    mb2.y = 0.0;
    ev2.data = mb2;

    editor.OnInput(ev1);
    editor.OnInput(ev2);

    EXPECT_EQ(CountPoints(reg), 2u);
    EXPECT_EQ(CountLines(reg), 1u);
}

// 5) Mode without tool does not crash and ignores input
TEST(EditorBasics, MissingToolIsGraceful) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    ToolContext ctx(reg, geom);

    Editor editor(ctx);
    editor.SetMode(EditorMode::Select);  // no tool registered

    InputEvent ev;
    ev.type = InputEventType::Key;
    KeyEvent ke;
    ke.key = 42;
    ke.pressed = true;
    ev.data = ke;

    // Should not crash, just no-op:
    editor.OnInput(ev);
}
