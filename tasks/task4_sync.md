# 📝 Implementierungsauftrag: Interaktiver Parametrischer Graph

Dieser Auftrag konzentriert sich auf die **Erstellung der UI-Tools** und **Commands** zur Steuerung des Graphen.

### 1\. 🖱️ Command-Implementierung (`nc::command`)

Implementiere die Befehle, die die Entities erstellen und verbinden.

#### 1.1 `InsertGlobalParameterCommand`

```cpp
// Erstellt die Entity, fuegt GlobalParameterComponent, NameComponent, UINodeComponent hinzu.
// Parameter: name, initialValue
```

#### 1.2 `InsertBoxCommand`

```cpp
// Implementiert die Logik aus dem letzten Schritt:
// 1. Erstellt die Haupt-Box-Entity (BoxComponent, BodyComponent, NameComponent, UINodeComponent).
// 2. Erstellt drei UNABHÄNGIGE Expression-Entities (STATIC_VALUE) fuer W, L, H.
// 3. Verlinkt die BoxComponent mit den Expression-Entities.
// Alle erstellten IDs muessen fuer undo() gespeichert werden.
```

#### 1.3 `ConnectExpressionCommand`

```cpp
// Der zentrale Befehl fuer das "Wiring" im Graph Editor.
// Parameter: targetExpressionEntityId, sourceParameterEntityId
// Logik: Speichert den alten Zustand der targetExpressionEntityId, bevor die sourceParameterEntityId als Referenz gesetzt wird.
```

#### 1.4 `UpdateParameterCommand`

```cpp
// Aktualisiert den Wert eines Global Parameters (oder einer statischen Expression).
// Parameter: targetEntityId, newValue
// Logik: Aktualisiert GlobalParameterComponent::value und erhoeht GlobalParameterComponent::version.
```

### 2\. 🖼️ `GraphEditorSystem` Erweiterung (`nc::ui`)

Implementiere die Logik für das Kontextmenü und das Link-Handling.

#### 2.1 Context Menu Logik (Aufruf von Commands)

Erweitere `GraphEditorSystem::drawPanel()`:

```cpp
// Nach ImNodes::EndNodeEditor()
if (ImGui::BeginPopupContextWindow()) {
    // 1. Global Parameter erstellen
    if (ImGui::MenuItem("Add Global Parameter")) {
        // Erzeuge Dialog/temporaeren Zustand zur Eingabe von Name/Wert, dann:
        // m_commandStack.executeCommand(std::make_unique<InsertGlobalParameterCommand>(...));
    }
    // 2. Box erstellen
    if (ImGui::MenuItem("Add Box Feature")) {
        // m_commandStack.executeCommand(std::make_unique<InsertBoxCommand>(...));
    }
    ImGui::EndPopup();
}
```

#### 2.2 Link Erstellung (Wiring)

Implementiere die Logik, um `ConnectExpressionCommand` zu erzeugen, wenn der Benutzer einen Link zieht.

```cpp
// Innerhalb von drawPanel() nach EndNodeEditor()
ImNodes::LinkData linkData;
if (ImNodes::IsLinkCreated(&linkData)) {
    // linkData enthaelt die IDs der Pins/Attribute.
    // Wir muessen eine Logik implementieren, die Attribute-ID zur EntityID der ExpressionComponent mappt.

    // Annahme: pinId -> EntityID Mapping im GraphEditorSystem.

    // Finde Target und Source EntityIDs und erstelle Command:
    // m_commandStack.executeCommand(std::make_unique<ConnectExpressionCommand>(targetId, sourceId));
}
```

#### 2.3 Name-Anzeige (Robustheit)

Stelle sicher, dass `drawFeatureNode()` und `drawParameterNode()` eine robuste Namensanzeige verwenden:

```cpp
std::string nc::ui::GraphEditorSystem::getNodeTitle(domain::EntityID entityId) {
    if (m_registry.hasComponent<domain::NameComponent>(entityId)) {
        return m_registry.getComponent<domain::NameComponent>(entityId).name; // m_name -> name
    }
    // Fallback:
    return "Unnamed [" + std::to_string(entityId) + "]";
}
```

---

**Fazit:** Die Implementierung dieser Commands und der UI-Event-Handler im **`GraphEditorSystem`** schließt den Kreislauf zur Interaktion und erfüllt alle Ihre Workflow-Ziele. Fahren Sie mit der Implementierung fort\!
