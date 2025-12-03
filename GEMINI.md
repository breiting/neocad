# 🌌 OntoFlow: Architectural Design Log (GEMINI.md)

**Identity:** An Ontological Engine for Computational Emergence.
**Philosophy:** Simplicity $\to$ Complexity. We build the machine that builds the model.
**Status:** MVP Construction Phase (Headless Core).

## ⚙️ Core Specifications

| Parameter        | Specification      | Notes                                                                            |
| :--------------- | :----------------- | :------------------------------------------------------------------------------- |
| **Namespace**    | `of::`             | Represents **O**nto**F**low. Sub-namespaces: `of::domain`, `of::engine`, etc.    |
| **Language**     | C++17              | Strict RAII, modern features (std::variant, std::optional).                      |
| **Architecture** | **Dataflow ECS**   | Entities are Nodes. Components define Logic & State. Systems evaluate the Graph. |
| **Geometry**     | OpenCascade (OCCT) | Encapsulated as a stateless service in `of::geometry`.                           |
| **Scripting**    | Lua 5.4            | Used for Agent Behavior and Runtime Node Definition.                             |
| **UI**           | ImGui + ImNodes    | Visualizes the Registry. The Graph is the primary interface.                     |

## 🧱 The Ontology (Data Model)

We abandoned specific component classes (like `BoxComponent`) in favor of a **Generic Node Model**.

### The Node Structure (`of::domain`)

Every functional unit in the system is an Entity with a **`NodeComponent`**.

```cpp
// inc/ontoflow/domain/NodeData.hpp & Components.hpp

enum class PinType { FLOW, FLOAT, INT, BOOL, VEC3, GEOMETRY, ANY };

struct Pin {
    std::string name;
    PinType type;
    std::variant<double, int, bool, glm::vec3, GeometryHandle> value;

    // Topology
    EntityID connectedNodeID = INVALID_ENTITY_ID;
    size_t connectedPinIdx = 0;
};

struct NodeComponent {
    std::string operationID; // E.g., "MATH_SIN", "GEO_BOX" (Keys into NodeRegistry)
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    bool isDirty = true;

    // Meta
    float uiX, uiY;
};
```

### Node Categories

1.  **Source Nodes:** Provide data (Time, Static Value, Imported Mesh).
2.  **Operator Nodes:** Transform data (Math, Logic).
3.  **Generator Nodes:** Create Matter (Box, Cylinder, AgentSpawner).
4.  **Sink Nodes:** Result/Export (RenderOutput, StepExport).

## 🏛️ System Architecture (The Engine)

### `of::domain` (The State)

- **`Registry`**: Pure Data. Stores Entities and Components.
- **`NodeComponent`**: The universal container for logic inputs/outputs.

### `of::engine` (The Logic)

- **`NodeRegistry` (The Factory):**
  - Maps `operationID` (string) to `NodeDefinition`.
  - `NodeDefinition` contains input/output templates and the **`evaluate` lambda**.
  - _Role:_ Knows HOW to calculate a "Box" or a "Sine Wave".
- **`GraphEvaluator` (The Runner):**
  - Traverses the DAG (Directed Acyclic Graph).
  - **Pull-Principle:** Starts at Sink Nodes -\> recursively solves dirty Inputs.
  - Calls `NodeRegistry::evaluateNode(...)`.

### `of::geometry` (The Service)

- **`OCCTBackend`**: Stateless functions.
  - Input: `double width, height, ...`
  - Output: `TopoDS_Shape` (wrapped).
- **`MeshGenerator`**: Converts TopoDS_Shape to Render-Mesh (Vertex Buffers).

### `of::ui` (The Interface)

- **`GraphEditorSystem`**:
  - Draws Nodes based on `NodeComponent` data via `ImNodes`.
  - Handles "Wiring" by dispatching Commands.
  - Does **not** execute logic.

---

## 🔄 Execution Flow: The "Pulse"

1.  **Mutation:** User changes a value or links a pin in UI.
2.  **Command:** `SetPinValueCommand` or `ConnectNodeCommand` updates the Registry.
3.  **Dirty Flag:** The modified Node is marked `isDirty = true`.
4.  **Evaluation Loop (`GraphEvaluator::update`):**
    - Finds active Output/Sink nodes.
    - Recursively checks dependencies.
    - If a dependency is dirty, re-runs its `evaluate` function from `NodeRegistry`.
    - Updates `OutputPins`.
5.  **Rendering:** `RenderingSystem` sees updated Geometry in the Sink Node and draws it.

## 🗺️ Roadmap: The MVP "The Flow"

**Goal:** A headless unit test proving the dataflow architecture.

| Step  | Task                                                                              | Status     |
| :---- | :-------------------------------------------------------------------------------- | :--------- |
| **1** | **Refactor File Tree:** Clean up legacy `nc::` files. Establish `of::` structure. | ⏳ Pending |
| **2** | **Domain Core:** Implement `NodeComponent` and `Pin` variant types.               | ⏳ Pending |
| **3** | **Engine Core:** Implement `NodeRegistry` (Factory) and register "Box" & "Sine".  | ⏳ Pending |
| **4** | **Evaluator:** Implement `GraphEvaluator` (The recursive pull-solver).            | ⏳ Pending |
| **5** | **Proof:** Write `main_test.cpp` connecting `Time` -\> `Sine` -\> `Box`.          | ⏳ Pending |
| **6** | **UI Integration:** Connect ImNodes to `NodeComponent`.                           | 🔮 Future  |

### 📝 Coding Guidelines

- **No specific component structs** (e.g., `BoxComponent` is banned). Use Generic `NodeComponent`.
- **Simplicity:** Prefer Composition over Inheritance.
- **Safety:** Use `std::variant` for Pin data. No `void*`.
- **Naming:** `CamelCase` for methods, `m_CamelCase` for private members.

## Coding Requirements

- Strong emphasis on clarity, modularity, simplicity and SOLID principles.
- Code must be header/source separated (.hpp / .cpp) and build cleanly on macOS and Linux (Clang, G++, C++17, OpenGL 4.1 Core Profile).
- .hpp go into inc/ folder (with sub-foldernames) and .cpp go into /src folder.
- All code should be enclosed in namespaces.
- Avoid singletons, globals, and magic numbers.
- Everything is designed to be extensible.
- All code should be tested with the Google Test framework.
- Use CMake and also use FetchContent for all external dependencies.

## Code Style

- Modern C++17 idioms: smart pointers (unique_ptr/shared_ptr), const correctness, override, RAII, STL containers.
- Mathematics if required: uses glm for all vector/matrix math
- For higher math, use `Eigen` at least in version 5.0
- OpenGL if required for rendering: explicit VAO/VBO setup, GLSL 410, clean shader uniforms, minimal state changes.
- ImGui if required for UI: used for all UI panels (fixed-width left control panel, dark pastel theme).
- File layout:
- Headers: declarations, doxygen-style documentation, minimal includes.
- Sources: full implementation, matching namespace. Can use `using namespace` in the beginning.
- Ideally one class per file.
- CamelCase() for public and private methods.
- m_CamelCase for private members.
- Only use structs for simple data container, otherwise use class.

## ClangFormat

BasedOnStyle: Google
---

Language: Cpp
IndentWidth: 4
ColumnLimit: 120
AllowShortFunctionsOnASingleLine: None

## Aesthetic & Design Preferences (if applicable)

- UI and visuals follow a Nord / pastel-dark palette with subtle gradients and shadows.
- UI is leaned on new Apple design with rounded corners.
- Split Update(dt) and Render() loop
- Shader effects should be elegant and minimalistic
- Code should be readable like a scientific paper, not a game script.

## Development Mindset

- Aim for scientific clarity, not hard-core performance hacks.
- Prefer explicit math formulas and well-documented reasoning.
- Each subsystem should be testable in isolation.

## What to produce

- Clean, modular, extensible C++ code following the above principles.
- Use design patterns whereever they make sense.
- Always explain theoretical context
- Use smart pointer (unique_ptr and shared_ptr) whenever possible and useful
