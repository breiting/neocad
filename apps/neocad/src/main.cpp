#include <memory>
#include <neocad/command/ExtrudeCommand.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/Editor.hpp>
#include <neocad/editor/InsertCircleTool.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <neocad/occt/OCCTBackend.hpp>
#include <neocad/ui/Window.hpp>
#include <neocad/vis/Camera2D.hpp>
#include <neocad/vis/Camera3D.hpp>
#include <neocad/vis/Mesh.hpp>
#include <neocad/vis/RenderingSystem.hpp>
#include <neocad/vis/StlReader.hpp>

#include "neocad/vis/OpenGLRenderer.hpp"

using namespace nc::domain;
using namespace nc::occt;
using namespace nc::cmd;
using namespace nc::editor;
using namespace nc::ui;
using namespace nc::vis;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr const char* APP_NAME = "neocad";

static KeyEvent MakeKeyEventFromGLFW(int key, int /*action*/, int mods) {
    using namespace nc;
    KeyEvent ev{};
    ev.pressed = true;
    ev.ctrl = (mods & GLFW_MOD_CONTROL) != 0;
    ev.alt = (mods & GLFW_MOD_ALT) != 0;
    ev.shift = (mods & GLFW_MOD_SHIFT) != 0;

    switch (key) {
        case GLFW_KEY_ESCAPE:
            ev.code = KeyCode::Escape;
            break;
        case GLFW_KEY_ENTER:
        case GLFW_KEY_KP_ENTER:
            ev.code = KeyCode::Enter;
            break;
        default:
            ev.code = KeyCode::Unknown;
            break;
    }

    ev.text = 0;
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        char base = ev.shift ? 'A' : 'a';
        ev.text = static_cast<char>(base + (key - GLFW_KEY_A));
    }
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && !ev.shift) {
        ev.text = static_cast<char>('0' + (key - GLFW_KEY_0));
    }

    return ev;
}

Entity LoadSTLtoECS(const std::string& file, Registry& ecs) {
    TriMesh mesh;
    StlReader reader;

    if (!reader.LoadFromFile(file, mesh)) {
        LOG(ERROR) << "Failed to load STL: " << file << "\n";
        return INVALID_ENTITY;
    }

    Entity e = ecs.CreateEntity();
    ecs.AddComponent<MeshComponent>(e, MeshComponent{mesh});
    ecs.AddComponent<NameComponent>(e, NameComponent{"ImportedSTL"});
    LOG(INFO) << "Successfully added entity: " << e;

    return e;
}

int main() {
    LOG(INFO) << "================================";
    LOG(INFO) << APP_NAME;
    LOG(INFO) << "================================";

    // ECS + Backend
    Registry registry;
    OCCTBackend backend;
    GeometrySystem geom(registry, backend);

    // Editor
    ToolContext ctx(registry, geom);
    Editor editor(ctx);
    editor.RegisterTool(EditorMode::InsertPoint, std::make_unique<InsertPointTool>());
    editor.RegisterTool(EditorMode::InsertLine, std::make_unique<SketchCurveTool>(CurveMode::Polyline));
    editor.RegisterTool(EditorMode::InsertCircle, std::make_unique<InsertCircleTool>());
    editor.RegisterTool(EditorMode::InsertSketch, std::make_unique<SketchCurveTool>(CurveMode::Face));

    // Window
    Window window;
    if (!window.Create({WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME}))
        return -1;

    // RenderingSystem (after Window initialization)
    auto renderer = std::make_unique<OpenGLRenderer>();
    RenderingSystem renderingSystem(std::move(renderer));

    // TODO
    auto cam2d = std::make_shared<Camera2D>();
    auto cam3d = std::make_shared<Camera3D>();
    editor.GetViewController().SetCameras(cam2d, cam3d);
    editor.GetViewController().SetViewportSize(window.GetWidth(), window.GetHeight());

    // INPUT MAPPING
    window.SetKeyPressedCallback([&](int key, int action) {
        InputEvent ev;
        ev.type = InputEventType::Key;
        ev.data = MakeKeyEventFromGLFW(key, action, 0);
        editor.OnInput(ev);
    });

    window.SetMouseButtonCallback([&](int btn, int act, int mods) {
        double x, y;
        glfwGetCursorPos(window.GetNative(), &x, &y);
        InputEvent ev;
        ev.type = InputEventType::MouseButton;
        ev.data = MouseButtonEvent{
            (btn == GLFW_MOUSE_BUTTON_LEFT ? MouseButton::Left : MouseButton::Right), act == GLFW_PRESS, {x, y}};
        editor.OnInput(ev);
    });

    window.SetMouseMoveCallback([&](double x, double y) {
        InputEvent ev;
        ev.type = InputEventType::MouseMove;
        MouseMoveEvent mm{};
        mm.position = {x, y};
        ev.data = mm;
        editor.OnInput(ev);
    });

    window.SetScrollCallback([&](double dx, double dy) {
        InputEvent ev;
        ev.type = InputEventType::Scroll;
        ScrollEvent sc{};
        sc.offset = {dx, dy};
        ev.data = sc;
        editor.OnInput(ev);
    });

    Entity stl = LoadSTLtoECS("body.stl", registry);
    if (stl == INVALID_ENTITY) {
        LOG(ERROR) << "Error during loading STL file";
        return -1;
    }

    auto lt = static_cast<float>(glfwGetTime());
    while (window.PollEvents()) {
        float ct = static_cast<float>(glfwGetTime());
        float dt = ct - lt;
        lt = ct;

        // UPDATE
        editor.Update(dt);
        renderingSystem.Update(registry);

        // RENDER
        renderingSystem.Render(editor.GetViewController().GetViewState());

        window.SwapBuffers();
    }

    window.Destroy();
    return 0;
}
