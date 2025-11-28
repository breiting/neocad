#include <CLI/CLI.hpp>
#include <memory>
#include <neocad/command/ExtrudeCommand.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/GeometrySystem.hpp>
#include <neocad/domain/IGeometryBackend.hpp>
#include <neocad/domain/PrimitiveFactory.hpp>
#include <neocad/domain/Query.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/Editor.hpp>
#include <neocad/editor/InsertCircleTool.hpp>
#include <neocad/editor/InsertPointTool.hpp>
#include <neocad/editor/SketchCurveTool.hpp>
#include <neocad/editor/ToolContext.hpp>
#include <neocad/occt/OCCTBackend.hpp>
#include <neocad/occt/STEPImporter.hpp>
#include <neocad/ui/Window.hpp>
#include <neocad/vis/Camera2D.hpp>
#include <neocad/vis/Camera3D.hpp>
#include <neocad/vis/Mesh.hpp>
#include <neocad/vis/OpenGLRenderer.hpp>
#include <neocad/vis/RenderingSystem.hpp>
#include <neocad/vis/StlReader.hpp>

#include "CLI/CLI.hpp"

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
    nc::domain::Mesh mesh;
    StlReader reader;

    if (!reader.LoadFromFile(file, mesh)) {
        LOG(Error) << "Failed to load STL: " << file << "\n";
        return INVALID_ENTITY;
    }

    Entity e = ecs.CreateEntity();
    ecs.AddComponent<MeshComponent>(e, MeshComponent{mesh});
    ecs.AddComponent<NameComponent>(e, NameComponent{"ImportedSTL"});
    LOG(Info) << "Successfully added entity: " << e;

    return e;
}

Entity CreateTestFace5(Registry& ecs) {
    using namespace nc::domain;

    // --- 1) 5 POSITIONEN (VERTICES) ---
    std::vector<glm::vec3> pts = {
        {0.0f, 0.0f, 0.0f}, {1.5f, 1.0f, 0.0f}, {2.0f, 2.2f, 0.0f}, {0.5f, 2.0f, 0.0f}, {-0.5f, 0.7f, 0.0f}};

    std::vector<Entity> vertexEntities;
    for (auto& p : pts) {
        Entity v = ecs.CreateEntity();
        ecs.AddComponent<PositionComponent>(v, {p});
        ecs.AddComponent<NameComponent>(v, {"Vertex"});
        vertexEntities.push_back(v);
    }

    // --- 2) EDGES ANLEGEN (jeweils v[i] → v[i+1]) ---
    std::vector<Entity> edgeEntities;
    for (size_t i = 0; i < vertexEntities.size(); ++i) {
        Entity e = ecs.CreateEntity();
        Entity v0 = vertexEntities[i];
        Entity v1 = vertexEntities[(i + 1) % vertexEntities.size()];  // close loop!

        ecs.AddComponent<EdgeComponent>(e, {v0, v1});
        ecs.AddComponent<NameComponent>(e, {"Edge"});
        edgeEntities.push_back(e);
    }

    // --- 3) FACEENTITY ANLEGEN ---
    Entity face = ecs.CreateEntity();
    FaceComponent fc;
    fc.vertices = vertexEntities;
    fc.edges = edgeEntities;

    ecs.AddComponent<FaceComponent>(face, std::move(fc));
    ecs.AddComponent<NameComponent>(face, {"TestFace5"});

    return face;
}

int main(int argc, char* argv[]) {
    CLI::App app{"Desc"};
    std::string stlFile;
    app.add_option("--stl", stlFile, "Load STL model");
    std::string stepFile;
    app.add_option("--step", stepFile, "Load STEP model");
    bool loadCube = false;
    app.add_flag("--cube", loadCube, "Load unit cube");
    bool loadFace = false;
    app.add_flag("--face", loadFace, "Load test face");

    CLI11_PARSE(app, argc, argv);

    LOG(Info) << "================================";
    LOG(Info) << APP_NAME;
    LOG(Info) << "================================";

    // ECS + Backend
    Registry registry;
    OCCTBackend backend;
    GeometrySystem geom(registry, backend);
    CommandStack cmdStack(registry, geom);

    if (loadCube) {
        Entity cube = PrimitiveFactory::MakeUnitCube(registry, "UnitCube");
        LOG(Info) << "Loaded unit cube with ID: " << cube;
    }
    if (loadFace) {
        Entity face = CreateTestFace5(registry);
        LOG(Info) << "Loaded face with ID: " << face;
    }

    if (!stlFile.empty()) {
        Entity stl = LoadSTLtoECS(stlFile, registry);
        if (stl == INVALID_ENTITY) {
            LOG(Error) << "Error loading STL file";
            return -1;
        }
    }

    if (!stepFile.empty()) {
        STEPImporter step;
        Entity mesh = step.Load(stepFile, registry);
        if (mesh == INVALID_ENTITY) {
            LOG(Error) << "Error loading STEP file";
            return -1;
        }
    }

    // Editor
    ToolContext ctx(registry, geom, cmdStack);
    Editor editor(ctx);
    editor.RegisterTool(EditorMode::InsertPoint, std::make_unique<InsertPointTool>());
    editor.RegisterTool(EditorMode::InsertLine,
                        std::make_unique<SketchCurveTool>(SketchCurveTool::CurveMode::Polyline));
    editor.RegisterTool(EditorMode::InsertSketch, std::make_unique<SketchCurveTool>(SketchCurveTool::CurveMode::Face));
    editor.RegisterTool(EditorMode::InsertCircle, std::make_unique<InsertCircleTool>());

    // Window
    Window window;
    if (!window.Create({WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME}))
        return -1;

    // RenderingSystem (after Window initialization)
    auto renderer = std::make_unique<OpenGLRenderer>();
    RenderingSystem renderingSystem(std::move(renderer));
    renderingSystem.Init(registry);
    renderingSystem.SetShowAxis(true);

    auto cam2D = std::make_shared<Camera2D>();
    auto cam3D = std::make_shared<Camera3D>();
    editor.SetCamera2D(cam2D);
    editor.SetCamera3D(cam3D);
    editor.SetViewportSize(window.GetWidth(), window.GetHeight());

    // INPUT MAPPING
    window.SetKeyPressedCallback([&](int key, int /*scancode*/, int action, int /*mods*/) {
        InputEvent ev;
        ev.type = InputEventType::Key;
        ev.data = MakeKeyEventFromGLFW(key, action, 0);
        editor.OnInput(ev);
    });

    window.SetMouseButtonCallback([&](int btn, int act, int /*mods*/) {
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

    window.SetWindowSizeCallback([&](int w, int h) {
        editor.SetViewportSize(w, h);
        renderingSystem.SetViewportSize(w, h);
    });

    auto lt = static_cast<float>(glfwGetTime());
    while (window.PollEvents()) {
        float ct = static_cast<float>(glfwGetTime());
        float dt = ct - lt;
        lt = ct;

        // UPDATE
        editor.Update(dt);
        renderingSystem.Update(registry);

        // RENDER
        auto* cam = editor.GetActiveCamera();
        assert(cam);
        renderingSystem.Render(cam);

        window.SwapBuffers();
    }

    window.Destroy();
    return 0;
}
