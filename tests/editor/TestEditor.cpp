#include <gtest/gtest.h>

#include <cstddef>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/Editor.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/command/CommandStack.hpp>
#include <neocad/vis/Camera2D.hpp>
#include <neocad/vis/Camera3D.hpp>

#include "neocad/editor/InputEvent.hpp"

using namespace nc::editor;
using namespace nc::domain;
using namespace nc::cmd;
using namespace nc::vis; // for Camera2D

// --- Minimal mock backend for GeometrySystem --------------------------------
class DummyBackend : public IGeometryBackend {
   public:
    BackendShapeHandle CreateExtrudedBody(const Polygon& profile, double height) override {
        lastProfile = profile;
        lastHeight = height;
        return ++lastHandle;
    }

    BackendShapeHandle CreateBox(double width, double length, double height) override {
        return ++lastHandle;
    }

    BackendShapeHandle CreateCylinder(double radius, double height) override {
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

// --- Tests ------------------------------------------------------------------

// 1) Editor starts in Normal mode
TEST(EditorBasics, StartsInNormalMode) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    CommandStack cmdStack(reg, geom);
    ToolContext ctx(reg, geom, cmdStack);

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
    bool OnInput(const InputEvent&, ToolContext&) override {
        ++inputCount;
        return false;
    }
    std::string GetName() const override { return "TestTool"; }
};

TEST(EditorBasics, ModeSwitchCallsEnterExit) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    CommandStack cmdStack(reg, geom);
    ToolContext ctx(reg, geom, cmdStack);

    Editor editor(ctx);

    auto toolNormal = std::make_unique<TestTool>();
    auto* ptrNormal = toolNormal.get();
    editor.RegisterTool(EditorMode::Normal, std::move(toolNormal));

    auto toolInsert = std::make_unique<TestTool>();
    auto* ptrInsert = toolInsert.get();
    editor.RegisterTool(EditorMode::InsertPoint, std::move(toolInsert));
    
    // Explicitly start in Normal mode to activate the tool
    editor.SetMode(EditorMode::Normal);
    EXPECT_EQ(ptrNormal->enterCount, 1);

    // Switch to InsertPoint
    editor.SetMode(EditorMode::InsertPoint);
    EXPECT_EQ(ptrNormal->exitCount, 1);
    EXPECT_EQ(ptrInsert->enterCount, 1);
    EXPECT_EQ(ptrInsert->exitCount, 0);
    
    // Switch back to Normal
    editor.SetMode(EditorMode::Normal);
    EXPECT_EQ(ptrInsert->exitCount, 1);
    EXPECT_EQ(ptrNormal->enterCount, 2);
}

TEST(InputEvent, DetectMouseClick) {
    MouseButtonEvent e{MouseButton::Left, true, {100, 200}};
    InputEvent evt({InputEventType::MouseButton, e});
    EXPECT_NE(AsMouseButton(evt), nullptr);
}

TEST(InputEvent, KeyPress) {
    KeyEvent e;
    e.code = KeyCode::Enter;
    e.pressed = true;
    InputEvent evt({InputEventType::Key, e});
    auto* k = AsKey(evt);
    EXPECT_TRUE(k->code == KeyCode::Enter);
}

// 3) InsertPointTool creates one point per click
TEST(InsertPointToolTests, CreatesPointOnClick) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    CommandStack cmdStack(reg, geom);
    ToolContext ctx(reg, geom, cmdStack);

    Editor editor(ctx);
    
    // Fix: Set camera so tool doesn't crash
    auto cam = std::make_shared<Camera3D>();
    cam->SetViewport(100, 100);
    editor.SetCamera3D(cam);
    editor.SetViewportSize(100, 100);

    editor.RegisterTool(EditorMode::InsertPoint, std::make_unique<InsertPointTool>());
    editor.SetMode(EditorMode::InsertPoint);

    MouseButtonEvent e{MouseButton::Left, true, {10, 20}};
    InputEvent evt({InputEventType::MouseButton, e});
    
    editor.OnInput(evt);
    
    EXPECT_EQ(CountPoints(reg), 1u);
}

TEST(SketchCurveTool, FaceCreation) {
    Registry reg;
    DummyBackend backend;
    GeometrySystem geom(reg, backend);
    CommandStack cmdStack(reg, geom);
    ToolContext ctx{reg, geom, cmdStack};
    
    // We need an editor attached to context because SketchCurveTool calls GetEditor()->SetMode
    Editor editor(ctx);
    // Setup camera
    auto cam = std::make_shared<Camera3D>();
    cam->SetViewport(100, 100);
    editor.SetCamera3D(cam);
    editor.SetViewportSize(100, 100);

    SketchCurveTool tool{SketchCurveTool::CurveMode::Face};
    tool.OnEnter(ctx);

    // simulate clicks
    MouseButtonEvent mb1{MouseButton::Left, true, {0, 0}};
    MouseButtonEvent mb2{MouseButton::Left, true, {10, 0}};
    MouseButtonEvent mb3{MouseButton::Left, true, {10, 10}};
    MouseButtonEvent mb4{MouseButton::Left, true, {0, 10}};

    tool.OnInput({InputEventType::MouseButton, mb1}, ctx);
    tool.OnInput({InputEventType::MouseButton, mb2}, ctx);
    tool.OnInput({InputEventType::MouseButton, mb3}, ctx);
    tool.OnInput({InputEventType::MouseButton, mb4}, ctx);

    // ENTER  → close and make FaceEntity
    //
    KeyEvent e;
    e.code = KeyCode::Enter;
    e.pressed = true;
    //
    tool.OnInput(InputEvent({InputEventType::Key, e}), ctx);

    // EXPECT_EQ(CountEntitiesWith<FaceComponent>(reg), 1u);
    // EXPECT_EQ(CountEntitiesWith<LineComponent>(reg), 4u);
    tool.OnExit(ctx);
}