# 🎯 Implementierungsauftrag: Minimal Parametric Box Prototyp (MRA)

**Ziel:** Implementierung des vollständigen parametrischen Kreislaufs (Parameter -> Expression -> Feature -> Rendering) mit einer Box, deren Maße von zwei globalen Parametern abhängen.

**Namespace-Regeln:** `nc::domain` für alle Komponenten und Systeme. `nc::occt` für das Backend.

**Coding-Style-Regeln:**

1.  **Structs:** Verwenden Sie keine `m_` Präfixe für Member in Structs (z.B. `value`, nicht `m_value`).
2.  **Classes:** Verwenden Sie `m_CamelCase` für private Member und `CamelCase()` für alle Methoden.
3.  **Typen:** Verwenden Sie `EntityID` aus `neocad/domain/Types.hpp`.

## 1. 📂 Komponenten-Definitionen (`inc/neocad/domain/Components.hpp`)

Implementiere die folgenden vier Komponenten.

```cpp
// Auszug aus nc::domain::Components.hpp

struct GlobalParameterComponent final {
    std::string name;
    double value = 0.0;
    uint64_t version = 0;
};

struct ExpressionComponent final {
    double evaluatedValue = 0.0;
    uint64_t version = 0;

    enum class SourceType { STATIC_VALUE, ENTITY_REFERENCE };
    SourceType sourceType = SourceType::STATIC_VALUE;

    // Wir nutzen nur double und EntityID fuer das MRA
    std::variant<double, EntityID> sourceData;
};

// Die Box-Definition (haelt Verweise auf die Expressions)
struct BoxComponent final {
    EntityID widthExpressionId;
    EntityID lengthExpressionId;
    EntityID heightExpressionId;
};

// BodyComponent: Speichert die Geometrie und die Version, auf der sie basiert.
struct BodyComponent final {
    // Annahme: ShapeWrapper ist ein RAII-Handle fuer TopoDS_Shape, definiert in nc::occt
    nc::occt::ShapeWrapper shape;
    uint64_t lastRebuildVersion = 0; // Hoechste Version der abhaengigen Expressions
};
```

## 2\. 📝 System 1: `ExpressionSystem` (Werte-Solver)

Implementiere das System, das den Werte-DAG auflöst und `ExpressionComponent::evaluatedValue` setzt.

#### `inc/neocad/domain/ExpressionSystem.hpp`

```cpp
#pragma once
#include "neocad/domain/Registry.hpp"

namespace nc { namespace domain {

class ExpressionSystem final {
public:
    ExpressionSystem(Registry& registry);
    void UpdateExpressions();

private:
    Registry& m_registry;
    // Helper fuer die rekursive Abarbeitung (im MRA nicht noetig, da nur 1-stufige Abhaengigkeit)
    void EvaluateExpression(EntityID expressionId);
};

}} // namespace nc::domain
```

#### `src/ExpressionSystem.cpp`

Implementiere `updateExpressions()`:

1.  Iteriere über alle Entities mit `ExpressionComponent`.
2.  Rufe `evaluateExpression(id)` auf.
3.  Implementiere `evaluateExpression()` so, dass sie die `sourceData` ausliest:
    - Wenn `ENTITY_REFERENCE`, prüfe die `version` der referenzierten `GlobalParameterComponent`.
    - Wenn die Target-Version höher ist als die lokale `version`, aktualisiere `evaluatedValue` und setze die lokale `version` auf die Target-Version.

## 3\. 🌳 System 2: `FeatureEvaluationSystem` (Geometrie-Rebuilder)

Implementiere das System, das die Geometrie erzeugt, wenn sich die Eingabewerte geändert haben.

#### `inc/neocad/domain/FeatureEvaluationSystem.hpp`

```cpp
#pragma once
#include "neocad/domain/Registry.hpp"
#include "neocad/occt/OCCTBackend.hpp"

namespace nc { namespace domain {

class FeatureEvaluationSystem final {
public:
    FeatureEvaluationSystem(Registry& registry, nc::occt::OCCTBackend& backend);
    void EvaluateFeatures();

private:
    Registry& m_registry;
    nc::occt::OCCTBackend& m_backend;

    void EvaluateBoxFeature(EntityID boxEntityId);

    // Helfer, um die hoeheste Version aus einer Liste von Expression-IDs zu finden
    uint64_t GetHighestExpressionVersion(std::initializer_list<EntityID> expressionIds);
};

}} // namespace nc::domain
```

#### `src/FeatureEvaluationSystem.cpp`

Implementiere `evaluateFeatures()`:

1.  Iteriere über alle Entities mit `BoxComponent` und `BodyComponent`.
2.  Rufe `evaluateBoxFeature(id)` auf.
3.  Implementiere `evaluateBoxFeature()`:
    - Hole die höchste `version` der drei abhängigen `ExpressionComponent`s.
    - Wenn `BodyComponent::lastRebuildVersion` kleiner ist als diese höchste Version:
      - Rufe `m_backend.makeBox(width, length, height)` mit den `evaluatedValue`s auf.
      - Aktualisiere `BodyComponent::shape` und setze `BodyComponent::lastRebuildVersion` auf die neue höchste Version.

---

### 4\. 🚀 Test-Setup in `main()` oder `TestScenario.cpp`

Schreibe ein kurzes Setup-Skript, um den Kreislauf zu testen:

1.  Erstelle **zwei** globale Parameter-Entities (`WIDTH`, `HEIGHT`).
2.  Erstelle die **Box-Entity**.
3.  Erstelle **drei** `ExpressionComponent`s für die Box (W, L, H):
    - W: `ENTITY_REFERENCE` zu `WIDTH`.
    - H: `ENTITY_REFERENCE` zu `HEIGHT`.
    - L: `STATIC_VALUE` = 15.0.
4.  Führe **`ExpressionSystem::updateExpressions()`** aus.
5.  Führe **`FeatureEvaluationSystem::evaluateFeatures()`** aus.
6.  **Ändere** `WIDTH` in der `GlobalParameterComponent`.
7.  Führe beide Update-Systeme erneut aus und **prüfe**, ob die Box im `BodyComponent` aktualisiert wurde (neue `lastRebuildVersion`).
