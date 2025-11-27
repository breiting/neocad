Kurzfassung:
• ECS = Wahrheit im Speicher (alle Punkte, Kanten, Faces, Bodies…)
• Node-Graph = “Wie wurde es erzeugt?” (Primitiven, Extrude, Boolean…)
• Lua & UI sind nur zwei verschiedene Frontends, die diesen Graphen / ECS befüllen.
• OCCT ist ein reiner Geometry-Worker.
• Rendering liest nur aus ECS und weiß nichts von OCCT oder Lua.

1. Schichten-Übersicht (Status neocad)

1.1. Domain / ECS (lib domain)
• Registry
• verwaltet Entities (IDs) und alle Components
• Components (Auswahl):
• PositionComponent { vec3 position }
• RadiusComponent { double r }
• EdgeComponent { Entity p0, p1 }
• FaceComponent { std::vector<Entity> vertices, edges }
• BodyComponent { /_ handle/ID ins Backend (OCCT-Shape) _/ }
• MeshComponent { TriMesh mesh } (trianguliert, fürs Rendering)
• SketchPlaneComponent { origin, normal, basisX, basisY }
• NameComponent, evtl. ScriptOwnedComponent, UserOwnedComponent usw.
• GeometrySystem
• sitzt auf der Registry
• kennt IGeometryBackend (z.B. OCCTBackend)
• API in Richtung “Domain-Befehle”, z.B.:
• Entity CreateBox(double x, y, z)
• Entity ExtrudeFace(Entity face, double height)
• Entity BooleanUnion(Entity a, Entity b)
• sorgt dafür:
• ruft Backend an → OCCT erzeugt TopoDS_Shape
• legt / updatet BodyComponent für die Entity
• trianguliert über Backend → MeshComponent
• Query
• unsere Filter-API (HasComponentQuery<T>, IsFace, IsBody etc.)
• wird von Editor und Rendering benutzt

🔑 Domain-Schicht kennt keine UI, kein OpenGL, kein Lua. Nur Geometrie, ECS, OCCT.

⸻

1.2. Geometry Backend (lib occt)
• IGeometryBackend
• Interface: CreateBox, Extrude, BooleanUnion, Triangulate, ExportSTEP…
• OCCTBackend : IGeometryBackend
• implementiert alles mit OpenCascade
• speichert intern TopoDS_Shape per ID oder Handle
• Domain sieht nur: “BodyComponent zeigt auf Backend-ID X”

🔑 Backend ist austauschbar (theoretisch andere Kernel denkbar).

⸻

1.3. Editor (lib editor)
• Editor
• hält:
• ToolContext (Registry + GeometrySystem)
• aktive Tools: InsertPointTool, SketchCurveTool, InsertCircleTool, …
• ViewController (steuert aktuelle Kamera & ViewMode)
• API:
• OnInput(InputEvent)
• Update(double dt)
• SetViewportSize(w, h) für Kameras
• ITool
• OnInput, OnUpdate
• manipuliert nur ECS über ToolContext
• ViewController
• lebt im editor, nicht in vis
• hält Zeiger auf ICamera (Interface)
• kennt nur:
• “aktuelle Kamera” (2D/3D)
• ViewMode (Sketch2D, View3D)
• Keyboard-Events (Num0/Num1 → Mode wechseln)
• gibt nach außen nur einen ViewState:
• glm::mat4 view
• glm::mat4 proj
• ViewMode mode
• ICamera
• Interface in editor (“view-level”, vis-agnostisch):
• GetViewMatrix(), GetProjectionMatrix()
• SetAspectRatio(float)
• Maus-Handler: OnMouseStart, OnMouseRotation, OnMousePan, OnMouseScroll
• Update(float dt)
• GetViewDirection()

🔑 Editor ist headless testbar: du kannst Tools & ViewLogik testen ohne OpenGL.

⸻

1.4. Visualisierung (lib vis)
• Camera2D, Camera3D
• implementieren editor::ICamera
• leben in nc::vis
• werden im main() konstruiert und dem Editor über ViewController::SetCamera2D/3D gegeben
• BaseGeometry
• abstrakte CPU→GPU-Geometrie
• Mesh, LineSet, PointSet leiten davon ab
• Material + Implementierungen:
• FlatShadedMaterial (Mesh, mit Licht)
• LineSetMaterial (2D/3D Linien)
• PointSetMaterial (instanced Points als Kreise)
• Renderer
• kennt:
• aktuelle Light
• aktuelle Material
• Methoden: DrawMesh, DrawLineSet, DrawPoints
• RenderingSystem
• Bindeglied zwischen ECS und Renderer
• hat:
• Renderer&
• std::unordered_map<Entity, std::shared_ptr<Mesh/LineSet/PointSet>>
• macht: 1. liest ECS via Query:
• MeshComponent → 3D-Bodies
• EdgeComponent → Liniengeometrie aufbauen
• FaceComponent → optional Face-Fill / Konturen
• PositionComponent → Punktwolke 2. baut/updated GPU-Objekte (Mesh, LineSet, PointSet) 3. ruft Renderer.Draw\* mit ViewState (view/proj) und model-Matrix

🔑 RenderingSystem kennt nur ECS + ViewState, nicht OCCT, nicht Lua.

⸻

1.5. Lua / Scripting (lib lua)
• CadAPI
• schön kleines Interface:
• CreatePoint(x,y,z), CreateBox(...), ExtrudeFace(...), Union(a,b), …
• intern benutzt GeometrySystem + Registry
• optional: baut auch den Node-Graph (s.u.)
• LuaEngine
• kapselt sol::state
• Initialize(), RunFile("model.lua")
• registriert Namespace nc:

nc.point(x,y,z)
nc.box(w,h,d)
nc.extrude(face, h)
nc.union(a, b)
nc.sketch.begin(...) :line_to(...) :face()

    •	alles, was aus Lua kommt, landet in der gleichen Registry wie UI-Interaktionen

→ aber erhält ScriptOwnedComponent statt UserOwnedComponent.

⸻

1.6. UI / App (lib ui + apps/neocad)
• Window
• GLFW/GLAD Wrapper, Callbacks für Maus/Tastatur/Resize
• ViewController wird über Key-Events gefüttert
• main.cpp verdrahtet alles:
• erzeugt: Registry, OCCTBackend, GeometrySystem, CadAPI, LuaEngine, Editor, Renderer, RenderingSystem, Kameras, Window
• Laufzeitloop:

while (window.PollEvents()) {
editor.Update(dt); // Tools, ViewController, etc.
viewController.Update(dt); // Cameras
auto vs = viewController.GetViewState();

    window.BeginFrame();
    renderingSystem.Render(registry, vs);
    window.EndFrame();

}

⸻

2. Node-Graph & “Bottom-Up” Aufbau

Das was du beschrieben hast (“Leaf → Ops → Root”) ist im Prinzip ein Operation Graph / DAG.

Wir brauchen dafür im domain eine zusätzliche Komponente:

enum class NodeKind {
PrimitiveBox,
PrimitiveSphere,
SketchFace,
Extrude,
BooleanUnion,
BooleanDiff,
Transform,
// ...
};

struct NodeComponent {
uint64_t nodeId; // stabil über Save/Load
NodeKind kind;
std::vector<uint64_t> inputs; // Node-IDs, von denen dieser abhängt
std::vector<Entity> outputs; // Entities (Body / Faces / etc)
bool scriptOwned; // aus model.lua?
};

Wie spielt das zusammen?
• Lua-Seite:
• nc.box(10,20,5):
• erzeugt neuen Node:
• kind = PrimitiveBox
• inputs = { }
• outputs = { entity_for_body }
• scriptOwned = true
• ruft GeometrySystem.CreateBox(...) → setzt BodyComponent + MeshComponent
• UI-Seite:
• User skizziert ein Face:
• Tool erzeugt Points, Edges, Face-Entity
• zusätzlich Node:
• kind = SketchFace
• inputs = {} oder {pointNodeIds}
• outputs = {faceEntity}
• scriptOwned = false (UserOwned)
• Extrude:
• UI oder Lua ruft extrude(face, h):
• Node: kind = Extrude, inputs = { nodeId_face }
• GeometrySystem.ExtrudeFace(...) erzeugt Body + optional OCCT-Shape → BodyComponent, MeshComponent.

Auswertung (Rebuild):

GeometrySystem::RebuildAll() könnte: 1. alle NodeComponents sammeln, 2. topologisch sortieren (nach inputs), 3. für jeden Node:
• je nach kind die passende Operation im Backend ausführen
• outputs-Entities updaten (Body/Mesh etc.)

Für den Anfang musst du es nicht voll durchdeklinieren, aber die Struktur passt perfekt zu deinem “bottom-up Graph”-Bild.

⸻

3. model.lua + model.data + ECS

Jetzt das Speichermodell:

3.1. model.data
• enthält ein Serialisat der Registry:
• alle Entities
• alle Components
• Positions, Faces, Bodies, Meshes, SketchPlanes, NodeComponents, ScriptOwned/UserOwned …
• beim Laden:
• Registry wird genau so wieder aufgebaut

3.2. model.lua
• enthält nur den parametrischen Teil:
• Parameter (param("width", 30))
• Konstruktionen (nc.box, nc.sketch, nc.union, …)
• beim Laden:
• Registry ist schon aus model.data gefüllt
• LuaEngine läuft durch:
• alles, was aus Script stammt:
• überschreibt / ersetzt nur ScriptOwned-Nodes & Entities
• UserOwned bleibt unangetastet
• danach optional: GeometrySystem::RebuildScriptOwned()

3.3. Drei Modi 1. nur model.lua
• kein model.data
• Registry wird komplett von Lua gefüllt
• Alles ist ScriptOwned 2. nur model.data
• Lua wird gar nicht geladen
• rein interaktives Modell (Parametrik evtl. nur im Constraint-Solver) 3. Hybrid
• model.data → UI/Script-Historie + User-Geometrie
• model.lua → generiert oder aktualisiert den Script-Teil
• ECS ist superposition aus beidem

⸻

4. Wie Rendering da reinpasst

Rendering weiß:
• MeshComponent → 3D-Bodies
• EdgeComponent → Linien
• FaceComponent + PositionComponent → Faces & Sketches
• PositionComponent → Punktwolke

Rendering weiß nicht:
• woher diese Entities kommen (Lua, UI, Backend)
• wie der Node-Graph aussieht
• was OCCT macht

Das ist perfekt, weil:
• du kannst später einen anderen Backend nehmen → Rendering bleibt gleich
• du kannst model.lua ändern → Rebuild → Validation & Rendering sehen die Resultate sofort

⸻

5. Constrains & Solver (kurz)

Die Constraints passen auch sehr gut in diese Architektur:
• Neue Components:
• ConstraintComponent (z.B. distance, angle, coincidence…)
• Neuer ConstraintSystem:
• liest ConstraintComponents und PositionComponents
• löst ein Gleichungssystem
• updated PositionComponents im ECS

Der Solver arbeitet direkt auf ECS.
Der Node-Graph ist eher “Feature-History”, der Solver stellt “geometrische Beziehungen” sicher.

⸻

6. TL;DR – “Gesamtkunstwerk” in einem Satz
   • ECS / Registry ist die Quelle der Wahrheit für den aktuellen Zustand.
   • Node-Graph (als NodeComponents) beschreibt “wie man dorthin kommt”.
   • Lua schreibt Script-Nodes (ScriptOwned), UI schreibt User-Nodes (UserOwned).
   • GeometrySystem + Backend wandeln Nodes → Bodies → Meshes.
   • RenderingSystem schaut nur auf die Components und zeichnet.
   • Editor ist die Interaktionsschicht, die Tools, View und Input organisiert – aber nichts von GL/Lua/OCCT weiß.
