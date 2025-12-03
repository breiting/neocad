## 🛠️ Task: Implement OntoFlow Engine Core (Registry & Evaluator)

**Context:**
We have established the `of::domain` layer with generic `NodeComponent` and `Pin`. Now we need the execution logic. We are implementing the **Pull-based Graph Evaluation** mechanism and the **Node Factory**.

**Goal:**

1.  Implement `NodeRegistry` (Factory & Logic Library).
2.  Implement `GraphEvaluator` (Recursive DAG Solver).
3.  Unit Test the flow: `ValueNode` + `ValueNode` -\> `AddNode` = Result.

### Part 1: The Node Registry (`of::engine`)

Create `inc/ontoflow/engine/NodeRegistry.hpp` and `src/NodeRegistry.cpp`.

**Responsibilities:**

- Singleton pattern for global access to node definitions.
- `registerNode(opID, definition)`: Store how a node looks and behaves.
- `spawnNode(registry, opID)`: Create an entity, attach `NodeComponent`, setup Pins based on the definition template, and return ID.

**Key Data Structure:**

```cpp
using NodeComputeFn = std::function<void(domain::NodeComponent& node, domain::Registry& reg)>;

struct NodeDefinition {
    std::string name;           // Display Name
    std::string category;       // Palette Category
    std::vector<domain::Pin> inputs;
    std::vector<domain::Pin> outputs;
    NodeComputeFn compute;      // The logic lambda
};
```

### Part 2: The Graph Evaluator (`of::engine`)

Create `inc/ontoflow/engine/GraphEvaluator.hpp` and `src/GraphEvaluator.cpp`.

**Responsibilities:**

- Recursive "Pull" evaluation.
- `evaluate(nodeID)`:
  1.  Check if node exists.
  2.  **Recursion:** Iterate over input pins. If a pin is connected (`targetNodeID != INVALID`), call `evaluate(targetNodeID)` first.
  3.  **Data Transfer:** After recursion returns, copy the result from the source node's Output Pin to the current node's Input Pin value.
  4.  **Compute:** Look up the `NodeDefinition` in `NodeRegistry` via `operationID`. Execute the `compute` lambda.
  5.  **State:** Set `isDirty = false`.

### Part 3: Unit Tests (`tests/EngineTests.cpp`)

Create a Google Test suite to verify the logic without UI.

**Test Case: "Math Addition Flow"**

1.  **Setup:** Register a "MATH_ADD" node definition.
    - Logic: `node.outputs[0].value = get<double>(node.inputs[0].value) + get<double>(node.inputs[1].value)`
2.  **Build Graph:**
    - Node A (Value): Output = 10.0
    - Node B (Value): Output = 32.0
    - Node C (Math Add): Input[0] connected to A, Input[1] connected to B.
3.  **Execute:** Call `evaluator.evaluate(C)`.
4.  **Assert:** Check if Node C's output pin value is `42.0`.

### Part 4: headless

Make a solid and feasible example in the app/headless/src/main.cpp which uses the new functionality, and performs a valid example in the CAD space.

### Documentation Requirements

- Add Doxygen-style comments (`///`) to all public methods in headers.
- Explain the "Pull" mechanism in `GraphEvaluator` comments.

**Deliverables:**
Please provide the C++ code for `NodeRegistry`, `GraphEvaluator`, and `EngineTests.cpp`. Ensure namespaces are `of::engine`. Make sure that the code builds, the unit tests are passing.
