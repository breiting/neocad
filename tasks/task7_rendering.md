## 🛠️ Task: Implement Tesselation & Interaction Loop

**Context:**
The headless dataflow works. Now we integrate it into the `ontoflow` application. We need to convert the calculated OCCT B-Rep (`BodyComponent`) into renderable data (`MeshComponent`) and allow real-time modification via keyboard.

**Goal:**

1.  Implement `OCCTBackend::GetMeshFromShape` (Tesselation).
2.  Implement `UpdateMeshes` logic (Body -\> Mesh).
3.  Setup the "Pulsating Box" graph in `Editor`.
4.  Add Keyboard Interaction to modify the graph and trigger re-evaluation.

### Part 1: OCCT Tesselation (`inc/ontoflow/occt/OCCTBackend.hpp` & `.cpp`)

Add a method to convert the internal B-Rep to our `Mesh` struct. Use the `Mesh` datastructure from `domain/Types.hpp`

```cpp
// In OCCTBackend.hpp
of::domain::Mesh GetMeshFromShape(BackendShapeHandle handle);

// In OCCTBackend.cpp implementation details:
// 1. Retrieve TopoDS_Shape from handle.
// 2. Call BRepMesh_IncrementalMesh(shape, deflection) to generate triangles.
// 3. Iterate TopExp_Explorer(shape, TopAbs_FACE).
// 4. Extract Poly_Triangulation.
// 5. Fill of::domain::Mesh (vertices with normals, indices).
// Note: Handle coordinate transformation and index offsets (OCCT is 1-based).
```

### Part 2: The Mesh Sync Logic (`src/Editor.cpp`)

We need a helper function (or a minimal System) in the Editor that syncs Bodies to Meshes.

```cpp
void Editor::SyncMeshes() {
    auto view = m_registry.getEntitiesWith<of::domain::BodyComponent>();

    for (auto entity : view) {
        // Check if MeshComponent is missing or needs update
        // (For MVP: We can just regenerate if Body exists, or add a dirty flag later)

        auto& body = m_registry.getComponent<of::domain::BodyComponent>(entity);

        // Tesselate via Backend
        of::domain::Mesh meshData = m_backend.GetMeshFromShape(body.handle);

        // Update/Add MeshComponent
        if (m_registry.hasComponent<of::domain::MeshComponent>(entity)) {
            m_registry.getComponent<of::domain::MeshComponent>(entity).mesh = std::move(meshData);
             // Signal Renderer that VBO needs update (e.g. version++)
        } else {
            m_registry.addComponent(entity, of::domain::MeshComponent{std::move(meshData)});
        }
    }
}
```

### Part 3: Editor Setup & Interaction Loop (`src/Editor.cpp`)

Modify `Editor::Initialize` and `Editor::Update` to build the graph and handle input.

**1. Initialize (The Graph Setup):**
Reuse the logic from the Headless test, but store the EntityID of the "Width" Value Node in a member variable `m_widthNodeID` so we can access it later.

_Refinement for `GEO_BOX` compute lambda:_
Update the lambda to **reuse** the body entity if it already exists, instead of creating a new one every frame (which would leak memory/entities).

```cpp
// Inside GEO_BOX compute:
EntityID bodyEnt;
if (std::holds_alternative<GeometryHandle>(node.outputs[0].value)) {
    // Reuse existing output entity
    bodyEnt = std::get<GeometryHandle>(node.outputs[0].value).id;
} else {
    // Create new
    bodyEnt = reg.CreateEntity();
    node.outputs[0].value = GeometryHandle{bodyEnt};
}
// Update BodyComponent on bodyEnt...
```

**2. Update Loop (The Interaction):**

```cpp
void Editor::Update(float dt) {
    // 1. Input Handling (Simple "J" / "K" for resizing)
    if (ImGui::IsKeyPressed(ImGuiKey_K)) {
        auto* node = m_registry.getComponent<of::domain::NodeComponent>(m_widthNodeID);
        // Assuming Output[0] holds the double value for VALUE_FLOAT node
        double val = std::get<double>(node->outputs[0].value);
        node->outputs[0].value = val + 5.0; // Increment

        // Mark downstream as dirty (For MVP, we might just evaluate everything)
        // or trigger manual evaluation of the Box Node.
        m_needsEvaluation = true;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_J)) {
        // Decrement logic...
        m_needsEvaluation = true;
    }

    // 2. Evaluation
    if (m_needsEvaluation) {
        // Evaluate the Box Node (Pull-Principle will pull the new Width)
        m_evaluator.Evaluate(m_boxNodeID);

        // 3. Tesselation
        SyncMeshes();

        m_needsEvaluation = false;
    }

    // 4. Render
    m_renderer.Render(); // Draws all MeshComponents
}
```

**Deliverables:**

1.  `OCCTBackend.cpp` implementation for `GetMeshFromShape`.
2.  Updated `Editor.cpp` with `SyncMeshes`, Graph Setup, and the Keyboard Interaction Loop.
3.  Make sure that the `ontoflow` app under `apps/ontoflow` works perfectly and shows the box which has been modelled. also make sure that the keyboardshortcut works as expected.
