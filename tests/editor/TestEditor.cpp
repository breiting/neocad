#include <gtest/gtest.h>

#include <neocad/domain/Components.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/Editor.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/SketchCurveTool.hpp>

#include "neocad/editor/InputEvent.hpp"

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
    editor.SetMode(EditorMode::InsertPoint);
    EXPECT_EQ(toolInsert->enterCount, 1);
    EXPECT_EQ(toolInsert->exitCount, 0);

    // Switch to InsertPoint
    editor.SetMode(EditorMode::InsertPoint);
    EXPECT_EQ(ptrNormal->exitCount, 1);
    EXPECT_EQ(ptrInsert->enterCount, 1);

    // Switch back to Normal
    editor.SetMode(EditorMode::Normal);
    EXPECT_EQ(ptrInsert->exitCount, 1);
    EXPECT_EQ(ptrNormal->enterCount, 2);  // entered again
}

TEST(InputEvent, DetectMouseClick) {
    MouseButtonEvent e{MouseButton::Left, 100, 200};
    InputEvent evt(e);
    EXPECT_TRUE(evt.IsLeftMouseClick());
}

TEST(InputEvent, KeyPress) {
    KeyEvent e{Key::Enter, KeyAction::Press};
    InputEvent evt(e);
    EXPECT_TRUE(evt.IsKeyPressed(Key::Enter));
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

    MouseButtonEvent e{MouseButton::Left, 10, 20};
    InputEvent ev(e);
    editor.OnInput(ev);
    EXPECT_EQ(CountPoints(reg), 1u);
}

TEST(SketchCurveTool, FaceCreation) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    ToolContext ctx{reg, geom};

    SketchCurveTool tool{CurveMode::Face};
    tool.OnEnter(ctx);

    // simulate clicks
    tool.OnInput(InputEvent(MouseButtonEvent({MouseButton::Left, 0, 0})), ctx);
    tool.OnInput(InputEvent(MouseButtonEvent({MouseButton::Left, 10, 0})), ctx);
    tool.OnInput(InputEvent(MouseButtonEvent({MouseButton::Left, 10, 10})), ctx);
    tool.OnInput(InputEvent(MouseButtonEvent({MouseButton::Left, 0, 10})), ctx);

    // ENTER  → close and make FaceEntity
    tool.OnInput(InputEvent(KeyEvent({Key::Enter, KeyAction::Press})), ctx);

    // EXPECT_EQ(CountEntitiesWith<FaceComponent>(reg), 1u);
    // EXPECT_EQ(CountEntitiesWith<LineComponent>(reg), 4u);
}
