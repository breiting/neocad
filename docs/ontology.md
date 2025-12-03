Ich möchte eine neue Generation von neocad entwerfen – ein minimalistisches, parametisches, ontologiegestütztes CAD- und Simulationssystem.
Bitte hilf mir, das System architektonisch, ontologisch und funktional sauber zu definieren.
Ich gebe dir jetzt die technischen Basisbausteine. Deine Aufgabe ist es, daraus eine klare, kohärente Systemarchitektur abzuleiten, die:
• minimalistisch
• modular
• präzise
• emergenzfähig
• erweiterbar
• auf „Simplicity → Complexity“ optimiert

ist.

1. Ziel des Systems

Das System soll nicht primär ein Produkt sein, sondern ein Denkwerkzeug, in dem geometrische, physikalische oder agentenbasierte Systeme modelliert werden können.
Es soll eine eigene Ontologie bereitstellen, mit der der Benutzer kleine Welten bauen kann: Formen, Parameter, Agenten, Regeln, Muster, Simulationen.

Es dient gleichzeitig:
• als geometrischer Kern (parametrisches CAD)
• als Simulationsraum (Regeln, Agenten, Prozesse)
• als exploratives Werkzeug (Nodes, Lua-Skripte, interaktive Modelle)

2. Technische Basis, die fix ist
   • C++
   • GLFW-basierte Render Engine (OpenGL)
   • OCCT für Geometrie
   • Lua als Metaprogramming/Script-Layer
   • ImGui für UI
   • Node Editor als zentrales Interface
   • modulare Pakete:
   • domain (Ontologie, Semantik)
   • commands (Operationen, Transforms)
   • editor (UI, Node-Graph, Interaction)
   • runtime (Execution Engine)
   • lua (Bindings, Reflective Layer)
   • render (Visualisierung)
   • occt-gateway (Modellkern)

3. Deine Aufgabe als KI

Entwirf eine saubere, konsistente Systemarchitektur, die Folgendes erzeugt:

A. Eine klare Ontologie

Definiere die grundlegenden Entitäten („was existiert“):
• Primitive (Point, Curve, Solid, Parameter)
• Modifier (Extrude, Union, Shell, …)
• Meta-Kategorien (Agent, Rule, Constraint)
• States, Attributes, Functions

Definiere die Beziehungen zwischen diesen Entitäten:
• Komposition
• Transformation
• Parametrik
• Abhängigkeiten
• Zeitabhängigkeit (optional)

Definiere, was im Editor „ein Node“ bedeutet und welche Node-Arten existieren (Value Node, Geometry Node, Rule Node, Agent Node, etc.).

B. Die Architektur des CAD-/Simulation-Systems

Baue ein modular aufgebautes System mit den Schichten: 1. Ontology Layer
– definiert Entitäten, Eigenschaften, Semantik, Constraints.
– ist UI- und Render-unabhängig. 2. Domain Layer
– enthält Typen, Regeln, Typenregister, Factories, Reflection-Metadaten. 3. Command Layer
– Operationen, die Zustände verändern (Undo/Redo, Transactions, History). 4. Runtime Layer
– führt Graphen, Scripte, Regeln, Parameteränderungen aus.
– Scheduling, dirty-flag, event system. 5. Lua Layer
– bidirektionale Bindings zu Domain + Runtime.
– scripting interface, metaprogramming. 6. Editor Layer (ImGui + Node Editor)
– UI, NodeGraph, Inspector, Timeline, Viewports. 7. Rendering Layer
– Visualisierung, GPU-Abstraktion, Shaders. 8. OCCT Layer
– BRep, TopoDS, boolean ops.
– über Gateways, nicht direkt im Domain Core.

C. Naming, Philosophy, Constraints

Alles soll auf folgenden Prinzipien basieren:
• Simplicity über Power
• Emergenz als Designziel
• Low-level Core, high-level Lua
• Ontologie > Implementierung
• No Magic: alles sichtbar, debuggable, transparent
• Open-World: der Nutzer kann Entitäten erweitern
• Data-Oriented Thinking (ECS optional)
• Compute Graph (Node Editor) als primäres Interface

D. Konkrete Deliverables

Erzeuge: 1. Eine vollständige, minimalistische Ontologie-Definition 2. Eine komplette Systemarchitektur (Layer für Layer) 3. Ein Komponenten-Diagramm 4. Ein Datenflussmodell 5. Eine Beschreibung der Node-Typen 6. Eine Beschreibung der Lua-Integration 7. Eine Beschreibung der OCCT-Integration 8. Ein Beispiel: Minimal World Construction 9. Ein Use-Case: Agenten + Geometrie als hybrides Modell 10. Eine Roadmap 2026: baubar in realistischen Schritten
