## 🛠️ Task: Refactor Legacy Code & Implement OntoFlow Domain Core

**Context:**
We are renaming the project from `neocad` (`nc::`) to **OntoFlow** (`of::`). We have deleted legacy components (`BoxComponent`, `GlobalParameterComponent`) and commands. The build is currently broken due to missing references.

**Goal:**

1.  **Fix Build:** Remove/Comment out all references to deleted classes in the `Editor` and `UI` layers to restore compilation.
2.  **Implement Domain:** Create the new generic `NodeComponent` and `Pin` data structures using strict SOLID principles.
3.  **Unit Test:** Add a Google Test to verify the Node data structure.

### Part 1: The Cleanup (Fix Compilation)

Please modify the following files to remove dependencies on deleted files (like `InsertBoxCommand.hpp`, `ExtrudeCommand.hpp`, etc.):

1.  **`inc/ontoflow/editor/Editor.hpp` & `src/Editor.cpp`**:
    - Remove includes to legacy commands.
    - Remove legacy member variables (e.g., `m_insertBoxTool`).
    - Keep the core loop and `GraphEditorSystem` instance, but ensure it initializes with the new namespace `of::`.

2.  **`inc/ontoflow/ui/GraphEditorSystem.hpp` & `src/GraphEditorSystem.cpp`**:
    - **Radical Cleanup:** Comment out the entire body of `DrawPanel` and `DrawNode` logic for now. We will reimplement this later using the new `NodeComponent`.
    - Remove includes for `Insert*Command.hpp`.
    - Ensure the class compiles as an empty shell waiting for the new logic.

3.  **`CMakeLists.txt` (Root & Subdirectories)**:
    - Update project name to `OntoFlow`.
    - Ensure the `domain` library targets the new source files defined below.

### Part 2: The Foundation (`of::domain`)

Implement the core data structures for the Dataflow architecture.

**1. `inc/ontoflow/domain/Types.hpp`**

- Make sure that we have `EntityID` defined and `INVALID_ENTITY_ID`.

**2. `inc/ontoflow/domain/NodeData.hpp`**

- Define `enum class PinType`.
- Define `struct GeometryHandle` (wrapper for EntityID).
- Define `using PinValue = std::variant<...>` (support double, int, bool, glm::vec3, GeometryHandle).
- Define `struct Connection` (targetNodeID, targetPinIdx).
- Define `struct Pin` (name, type, value, connection).

**3. `inc/ontoflow/domain/Components.hpp`**

- Define `struct NodeComponent`.
  - Members: `std::string operationID`, `std::vector<Pin> inputs`, `std::vector<Pin> outputs`, `bool isDirty`.
  - UI Meta: `float uiX`, `float uiY`.
- Define `struct NameComponent` (std::string name).
- Define `struct BodyComponent` (placeholder for OCCT shape wrapper).

### Part 3: Unit Testing

Create **`tests/DomainTests.cpp`** to verify the data structure behaves as expected.

```cpp
#include <gtest/gtest.h>
#include "ontoflow/domain/Components.hpp"
#include "ontoflow/domain/Registry.hpp" // Assuming Registry is updated to of::

TEST(NodeSystem, PinValueAssignment) {
    of::domain::Pin inputPin;
    inputPin.type = of::domain::PinType::FLOAT;
    inputPin.value = 42.0;

    ASSERT_TRUE(std::holds_alternative<double>(inputPin.value));
    EXPECT_DOUBLE_EQ(std::get<double>(inputPin.value), 42.0);
}

TEST(NodeSystem, NodeComponentStructure) {
    of::domain::NodeComponent node;
    node.operationID = "MATH_ADD";

    of::domain::Pin p1; p1.name = "A"; p1.type = of::domain::PinType::FLOAT;
    of::domain::Pin p2; p2.name = "B"; p2.type = of::domain::PinType::FLOAT;

    node.inputs.push_back(p1);
    node.inputs.push_back(p2);

    EXPECT_EQ(node.inputs.size(), 2);
    EXPECT_EQ(node.inputs[0].name, "A");
}
```

**Deliverables:**
Please produce the code for `NodeData.hpp`, `Components.hpp`, the cleaned `GraphEditorSystem.cpp`, and the `DomainTests.cpp`.
