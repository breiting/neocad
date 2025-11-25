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

using namespace nc::domain;
using namespace nc::occt;
using namespace nc::cmd;
using namespace nc::editor;
using namespace nc::ui;

static KeyEvent MakeKeyEventFromGLFW(int key, int action, int mods) {
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
    // Buchstaben (vereinfachte Variante, US-Layout)
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        char base = ev.shift ? 'A' : 'a';
        ev.text = static_cast<char>(base + (key - GLFW_KEY_A));
    }
    // Ziffern
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && !ev.shift) {
        ev.text = static_cast<char>('0' + (key - GLFW_KEY_0));
    }

    return ev;
}

int main() {
    LOG(INFO) << "================================";
    LOG(INFO) << "neoCAD";
    LOG(INFO) << "================================";

    Registry registry;
    OCCTBackend backend;  // deine konkrete Backend-Implementierung
    GeometrySystem geom(registry, backend);
    ToolContext ctx(registry, geom);
    Editor editor(ctx);

    editor.RegisterTool(EditorMode::InsertPoint, std::make_unique<InsertPointTool>());
    editor.RegisterTool(EditorMode::InsertLine, std::make_unique<SketchCurveTool>(CurveMode::Polyline));
    editor.RegisterTool(EditorMode::InsertCircle, std::make_unique<InsertCircleTool>());
    editor.RegisterTool(EditorMode::InsertSketch, std::make_unique<SketchCurveTool>(CurveMode::Face));

    Window::CreateInfo ci;
    Window window;
    if (!window.Create(ci))
        return -1;

    // KEY
    window.SetKeyPressedCallback([&](int key, int action) {
        InputEvent ev;
        ev.type = InputEventType::Key;
        ev.data = MakeKeyEventFromGLFW(key, action, /*mods fehlt hier*/ 0);
        editor.OnInput(ev);
    });

    // MOUSE BUTTON
    window.SetMouseButtonCallback([&](int button, int action, int mods) {
        (void)mods;
        double x, y;
        glfwGetCursorPos(window.GetNative(), &x, &y);

        InputEvent ev;
        ev.type = InputEventType::MouseButton;

        MouseButtonEvent mb{};
        mb.button = (button == GLFW_MOUSE_BUTTON_RIGHT) ? MouseButton::Right : MouseButton::Left;
        mb.pressed = (action == GLFW_PRESS);
        mb.position = {x, y};
        ev.data = mb;

        editor.OnInput(ev);
    });

    // MOUSE MOVE
    window.SetMouseMoveCallback([&](double x, double y) {
        InputEvent ev;
        ev.type = InputEventType::MouseMove;
        MouseMoveEvent mm{};
        mm.position = {x, y};
        ev.data = mm;
        editor.OnInput(ev);
    });

    // SCROLL
    window.SetScrollCallback([&](double dx, double dy) {
        InputEvent ev;
        ev.type = InputEventType::Scroll;
        ScrollEvent sc{};
        sc.offset = {dx, dy};
        ev.data = sc;
        editor.OnInput(ev);
    });

    while (window.PollEvents()) {
        editor.Update(1.0 / 60.0);
        window.BeginFrame();
        // TODO: rendering
        window.EndFrame();
    }

    window.Destroy();
    return 0;
}
