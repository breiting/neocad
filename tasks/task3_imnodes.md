## 📝 Implementierungsauftrag: GraphEditor & ImNodes

Dieser Auftrag umfasst die Integration von ImNodes, die Erstellung des `GraphEditorSystem` als unabhängiges Fenster und die Implementierung des Toggle-Mechanismus über den `Editor`.

### 1\. 🌐 Integration & Setup

1.  **ImNodes via FetchContent:** Fügen Sie **`ImNodes`** als externe Abhängigkeit über `FetchContent` hinzu.
2.  **ImGui/ImNodes Context:** Initialisieren Sie den **`ImNodes Context`** (`ImNodes::CreateContext()`) im Konstruktor der Haupt-UI-Klasse (z.B. `nc::editor::Editor`) und zerstören Sie ihn beim Beenden.

### 2\. 🧩 Komponenten-Erweiterung (`inc/neocad/domain/Components.hpp`)

Fügen Sie die Komponente zur Speicherung der UI-Position des Knotens hinzu.

```cpp
// inc/neocad/domain/Components.hpp

namespace nc { namespace domain {

// Speichert die Position des Nodes im 2D-Graph-Editor-Fenster (fuer Persistenz)
struct UINodeComponent final {
    float positionX = 0.0f;
    float positionY = 0.0f;
    bool isCollapsed = false;
};

// ... weitere Komponenten ...

}} // namespace nc::domain
```

### 3\. 🖼️ Das `GraphEditorSystem`

#### `inc/neocad/ui/GraphEditorSystem.hpp` (Schnittstelle)

Das System erhält eine Methode zur Steuerung seiner Sichtbarkeit.

```cpp
#pragma once
// ... notwendige Includes (Registry, Types, ImNodes) ...

namespace nc { namespace ui {

class GraphEditorSystem final {
public:
    GraphEditorSystem(domain::Registry& registry);

    void DrawPanel(); // Wird vom Editor aufgerufen, um das ImGui Fenster zu zeichnen
    void ToggleVisibility(); // NEU: Steuert das separate Fenster
    bool IsVisible() const { return m_isVisible; } // NEU

private:
    domain::Registry& m_Registry;
    bool m_IsVisible = false; // Initial unsichtbar

    // Hilfsmethoden zum Zeichnen der verschiedenen Knotentypen
    void DrawParameterNode(domain::EntityID parameterId);
    void DrawFeatureNode(domain::EntityID featureId);
    void DrawPinAndInput(domain::EntityID expressionId, const std::string& label, bool isInput);
};

}} // namespace nc::ui
```

#### `src/GraphEditorSystem.cpp` (Implementierung)

1.  **`ToggleVisibility()`:** Implementieren Sie die einfache Umschaltfunktion für `m_isVisible`.
2.  **`drawPanel()`:** Implementieren Sie die Logik, die **NUR** dann `ImGui::Begin("Graph Editor")` und die ImNodes-Aufrufe durchführt, wenn `m_isVisible` **`true`** ist.
    - Verwenden Sie `ImGui::Begin("NeoCAD Graph Editor", &m_isVisible, ImGuiWindowFlags_None)`. Das \&m_isVisible erlaubt das Schließen des Fensters durch den Benutzer über das X-Icon, was automatisch den internen Zustand aktualisiert.
    - Implementieren Sie die **drei Demo-Knoten** (1 Global Parameter, 2 Features) und einen **Demo-Link** zwischen ihnen, wie zuvor beschrieben.

### 4\. 🚀 Editor Orchestrierung (Shortcut-Handling)

Der Haupt-`Editor` muss den Shortcut abfangen und das `GraphEditorSystem` steuern.

#### `inc/neocad/editor/Editor.hpp` (Erweiterung)

Fügen Sie das `GraphEditorSystem` als Member hinzu.

```cpp
namespace nc { namespace editor {

class Editor final {
// ...
private:
    // ... andere Systeme ...
    std::unique_ptr<ui::GraphEditorSystem> m_GraphEditorSystem; // NEU
};

}}
```

#### `src/Editor.cpp` (Update-Loop)

Implementieren Sie die Logik zur Steuerung der Sichtbarkeit im Haupt-Loop (angenommen, es existiert eine `InputSystem`- oder `KeyInput`-Abstraktion):

```cpp
void nc::editor::Editor::update() {
    // 1. Eingabe pruefen (Z.B. G-Taste)
    // Wir definieren KEY_G als den Shortcut fuer den Graphen
    if (m_inputSystem->isKeyToggled(KEY_G)) { // Annahme
        m_graphEditorSystem->toggleVisibility();
    }

    // 2. Commands und ECS-Systeme laufen lassen
    // ...
}

void nc::editor::Editor::draw() {
    // ...
    // Rendern der 3D Szene
    // ...

    // Start ImGui Frame
    // ...

    // 3. Zeichne den Graphen (wenn sichtbar)
    m_graphEditorSystem->drawPanel();

    // 4. Zeichne andere Panels
    // ...

    // Finalisiere ImGui
    // ...
}
```

Dieser Plan integriert die optische Validierung, die **Multi-Monitor-Fähigkeit** und die ergonomische Steuerung über einen Shortcut in einem einzigen, sauberen Architekturschritt. Fahren Sie mit der Implementierung fort.

II

# 📝 Implementierungsauftrag: ImNodes & Demo-Graph

Dieser Auftrag umfasst die Integration von **ImNodes** über `FetchContent`, die saubere architektonische Einbindung und die Erstellung eines Demo-Graphen zur visuellen Validierung.

### 1\. 🌐 Integration & Setup

1.  **ImNodes via FetchContent:** Füge **`ImNodes`** (die leichtgewichtige Node-Graph-Bibliothek für ImGui) als externe Abhängigkeit über `FetchContent` in der Haupt- `CMakeLists.txt` und im `ui/CMakeLists.txt` hinzu.
2.  **ImGui Context:** Initialisiere den **`ImNodes Context`** (mittels `ImNodes::CreateContext()`) an einer zentralen Stelle (wahrscheinlich im `Editor`- oder `Window`-Konstruktor) und zerstöre ihn beim Beenden.

### 2\. 🧩 Komponenten-Erweiterung (`inc/neocad/domain/Components.hpp`)

Füge die Komponente hinzu, die die Node-Position speichert, damit der Graph seinen Layout beibehält.

```cpp
// inc/neocad/domain/Components.hpp

namespace nc { namespace domain {

// Speichert die Position des Nodes im 2D-Graph-Editor-Fenster (fuer Persistenz)
struct UINodeComponent final {
    float positionX = 0.0f;
    float positionY = 0.0f;
    bool isCollapsed = false;
};

// ... weitere Komponenten ...

}} // namespace nc::domain
```

### 3\. 🖼️ Das `GraphEditorSystem`

#### `inc/neocad/ui/GraphEditorSystem.hpp`

Definiere die Schnittstelle des Systems.

```cpp
#pragma once

#include "neocad/domain/Registry.hpp"
#include "neocad/domain/Types.hpp"
#include <string>

// Vorwaertsdeklaration fuer den ImGui Context und ImNodes
namespace ImNodes {
    using Context = void;
}

namespace nc { namespace ui {

class GraphEditorSystem final {
public:
    // Benötigt Registry, um Komponenten zu lesen (die Wahrheit!)
    GraphEditorSystem(domain::Registry& registry);

    void drawPanel(); // Haupt-Aufruf durch den Editor

    // Hilfsmethoden zum Zeichnen der verschiedenen Knotentypen
    void drawParameterNode(domain::EntityID parameterId);
    void drawFeatureNode(domain::EntityID featureId);

private:
    domain::Registry& m_registry;

    // Interne Helper-Funktion zum Zeichnen der Pins und Input-Felder
    void drawPinAndInput(domain::EntityID expressionId, const std::string& label, bool isInput);
};

}} // namespace nc::ui
```

#### `src/GraphEditorSystem.cpp` (Demo-Logik)

Implementiere die Logik, um die Demo-Knoten und Links zu zeichnen.

1.  **`drawPanel()` Implementierung:**
    - Rufe `ImGui::Begin("NeoCAD Graph Editor")` auf.
    - Rufe `ImNodes::BeginNodeEditor()` auf.
    - **Iteriere und Zeichne:** Durchlaufe alle Entities mit **`GlobalParameterComponent`** und **`BoxComponent`** (oder **`FeatureComponent`**).
    - Rufe `ImNodes::EndNodeEditor()` und `ImGui::End()` auf.

2.  **Demo-Knoten-Erzeugung (Im `Editor` Setup):**
    - Erstelle im Initialisierungscode des `Editor` **drei Demo-Entities** (einen Parameter und zwei Box-Features) und füge ihnen `UINodeComponent`s hinzu.
    - **Simuliere einen Link:** Füge manuell einen Link-Befehl für ImNodes hinzu, der die `Box.width` an den `GlobalParameter` bindet (um die Visualisierung der Abhängigkeit zu sehen).

3.  **Ästhetik-Anpassung:** Verwende `ImNodes::PushColorStyle()` um die Pins für `ExpressionComponent`s (z.B. für Input/Output) farblich vom Haupt-Node abzuheben.
