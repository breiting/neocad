# 💎 neocad Architectural Design (GEMINI.md)

## Context

neocad is a new kind of CAD system — built from scratch with a radically simple philosophy:

### Core Principles

- Simple & minimalistic – no clutter, no heavy UI, no hidden magic.
- Fast and lightweight – focused on fundamentals, not complexity.
- Inspired by Neovim – modal editing, keyboard-driven workflows, scriptable and customizable.
- Designed for makers, hobbyists, designers, engineers – not corporate CAD departments.
- Built for repeatability – everything can be parametric and regenerated.
- Open-source

### How You Can Use neocad

You can build your design in three different ways — or mix them:

- Script-based model.lua — DSL to define geometry parametrically
- UI-based point-and-click, 2D/3D sketching tools
- Hybrid both worlds: UI changes generate code, scripts modify UI

This means:
👉 You can design visually,
👉 you can automate with code,
👉 or let both interact seamlessly.

### What Makes neocad Different?

- Minimal, intuitive, fast
- Node-graph / ECS-based geometry engine
- Fully parametric — everything is reproducible
- Live geometry updates
- No “file format trap” — full control over your model
- Merges CAD + programming naturally
- Designed to be a tool — not a product

### Target Users

neocad is for:

- Makers / 3D printing enthusiasts
- Product designers
- DIY creators
- Robotics & hardware developers
- Small engineering teams
- Coders who love parametric control
- Anyone who wants freedom, not bloated CAD software

### Long-Term Vision

neocad should become:

“The Neovim of CAD.”
A powerful, elegant, scriptable tool — where geometry becomes code,
and CAD becomes creative and fun again.

## Core Specifications & Environment

| Parameter             | Specification                     | Notes                                                          |
| :-------------------- | :-------------------------------- | :------------------------------------------------------------- |
| **Project Name**      | **neocad**                        | Next-Generation Parametric CAD                                 |
| **Base Namespace**    | `nc::`                            | Sub-namespaces reflect modules (e.g., `nc::domain`, `nc::vis`) |
| **Language**          | C++17                             | Clang / G++ (macOS & Linux)                                    |
| **Coordinate System** | **Z-Up**                          | Standard for 3D modeling and rendering in neocad.              |
| **Geometry Kernel**   | OpenCascade (OCCT 7.9.1)          | Fully abstracted behind `nc::domain::IGeometryBackend`.        |
| **Modeling Type**     | Parametric, Script-Driven, Hybrid | Core logic relies on the ECS-based Feature Graph.              |

## Architectural Principles & Modules

### 2.1 Entity Component System (ECS)

- **Source of Truth:** The ECS Registry (`nc::domain::Registry`) is the central source of all model data.
- **Undo/Redo:** Strictly implemented using the **Command Design Pattern** via the **`nc::command::CommandStack`**. Tools must not modify the Registry directly; all state changes must be encapsulated in an `ICommand`.
- **Dependency Inversion Principle (DIP):** The `nc::domain` module **must not** directly depend on OCCT types. All geometry manipulation logic relies on the abstract interface `nc::domain::IGeometryBackend`.
- **Logger:** Only the `nc::core::Logger` is permitted to use the Singleton pattern.

### 2.2 Core Module Responsibilities

- **`core`:** Basic utilities which have no dependencies and can be used by every other package
- **`domain`:** ECS primitives, core Components. Defines the mathematical/parametric model.
- **`occt`:** Implements `IGeometryBackend`, wrapping all specific OCCT calls (B-Rep, meshing, importing).
- **`command`:** Contains the `ICommand` interface and the `CommandStack` for history management.
- **`vis`:** Handles all OpenGL, rendering, shading, and ray-casting logic.
- **`editor`:** Independent of rendering and offers all the tools modifying the data.
- **`lua`:** The script engine exposing a `CadAPI` to the Lua world
- **`ui`:** The user interface layer offering the actual event source and windowing with GLFW

## ECS Component Catalog (Core Definitions)

The ECS is the core of the system, and stores all information in the `Registry`

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

## Tool preferences

- Use context7 MCP server for any time you need documentation for external services
