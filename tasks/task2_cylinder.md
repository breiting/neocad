# 📝 Gemini-CLI Implementierungsauftrag: Box & Cylinder (Feature-Erweiterung)

Dieser Auftrag implementiert nun die notwendigen Komponenten, Commands und die Logik zur Evaluierung für Box und Zylinder.

### 1\. 📦 Komponenten-Erweiterung (`inc/neocad/domain/Components.hpp`)

Erweitere die Datei um die Zylinder-Komponente:

```cpp
// ... GlobalParameterComponent, ExpressionComponent, BodyComponent bleiben ...

// Die Box-Definition (existiert schon)
struct BoxComponent final {
    EntityID widthExpressionId;
    EntityID lengthExpressionId;
    EntityID heightExpressionId;
};

// Die NEUE Zylinder-Definition
struct CylinderComponent final {
    EntityID radiusExpressionId;
    EntityID heightExpressionId;
};
```

### 2\. 🧱 `OCCTBackend` Erweiterung (`inc/neocad/occt/OCCTBackend.hpp`)

Das Backend muss die primitiven Konstruktoren bereitstellen.

```cpp
// Auszug aus nc::occt::OCCTBackend.hpp
namespace nc { namespace occt {

class OCCTBackend final {
public:
    // ... existierende Methoden ...

    ShapeWrapper MakeBox(double w, double l, double h);

    // Neuer Aufruf fuer den Zylinder
    ShapeWrapper MakeCylinder(double radius, double height);

    // ...
};

}} // namespace nc::occt
```

### 3\. 🌳 `FeatureEvaluationSystem` Erweiterung (`src/FeatureEvaluationSystem.cpp`)

Erweitere das System, um beide Features zu evaluieren.

#### `inc/neocad/domain/FeatureEvaluationSystem.hpp` (Erweiterung)

Füge die neue private Methode hinzu:

```cpp
// ... in FeatureEvaluationSystem class ...
private:
    // ...
    void EvaluateBoxFeature(EntityID boxEntityId);
    void EvaluateCylinderFeature(EntityID cylinderEntityId); // NEU
};
```

#### `src/FeatureEvaluationSystem.cpp` (Logik-Erweiterung)

Implementiere die Logik für Zylinder (analog zur Box) und erweitere `EvaluateFeatures()`:

```cpp
void nc::domain::FeatureEvaluationSystem::EvaluateFeatures() {
    // 1. Box Evaluation
    m_registry.view<BoxComponent, BodyComponent>().each([this](auto entity, auto& boxComp, auto& bodyComp) {
        this->evaluateBoxFeature(entity);
    });

    // 2. Cylinder Evaluation (NEU)
    m_registry.view<CylinderComponent, BodyComponent>().each([this](auto entity, auto& cylinderComp, auto& bodyComp) {
        this->evaluateCylinderFeature(entity);
    });

    // ... weitere Features folgen hier ...
}

void nc::domain::FeatureEvaluationSystem::EvaluateCylinderFeature(EntityID cylinderEntityId) {
    // Implementiere Logik:
    // 1. Hole ExpressionComponenten fuer Radius und Hoehe (cylinderComp.radiusExpressionId, etc.)
    // 2. Finde die maxVersion.
    // 3. Wenn bodyComp.lastRebuildVersion < maxVersion:
    //    a. Rufe m_backend.makeCylinder(radiusValue, heightValue) auf.
    //    b. Aktualisiere bodyComp.shape und bodyComp.lastRebuildVersion.
}
```

### 4\. 🖱️ Commands (`nc::command`)

Implementiere die Commands, die die Feature-Entities erstellen und die Parameter auf statische Werte setzen.

#### `inc/neocad/command/InsertBoxCommand.hpp` / `src/InsertBoxCommand.cpp`

Definiere das `InsertBoxCommand`, das eine **Box-Entity** und **drei statische Expression-Entities** erstellt, und alle IDs für `undo()` speichert.

#### `inc/neocad/command/InsertCylinderCommand.hpp` / `src/InsertCylinderCommand.cpp`

Definiere das `InsertCylinderCommand`, das eine **Cylinder-Entity** und **zwei statische Expression-Entities** erstellt, und alle IDs für `undo()` speichert.

Schreibe sauber einen Unit test auch fuer das feature Cylinder.
