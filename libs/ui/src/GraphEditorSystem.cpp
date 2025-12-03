#include <imgui.h>
#include <imnodes.h>

#include <algorithm>
#include <neocad/command/ConnectExpressionCommand.hpp>
#include <neocad/command/InsertBoxCommand.hpp>
#include <neocad/command/InsertCylinderCommand.hpp>
#include <neocad/command/InsertGlobalParameterCommand.hpp>
#include <neocad/command/UpdateParameterCommand.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/ui/GraphEditorSystem.hpp>
#include <string>

namespace nc::ui {

GraphEditorSystem::GraphEditorSystem(domain::Registry& registry, nc::cmd::CommandStack& commandStack)
    : m_Registry(registry), m_CommandStack(commandStack) {
}

void GraphEditorSystem::ToggleVisibility() {
    m_IsVisible = !m_IsVisible;
}

void GraphEditorSystem::DrawPanel() {
    LOG(Info) << "GraphEditorSystem::DrawPanel() called. m_IsVisible: " << m_IsVisible;
    if (!m_IsVisible)
        return;

    if (ImGui::Begin("NeoCAD Graph Editor", &m_IsVisible)) {
        LOG(Info) << "ImGui::Begin succeeded for NeoCAD Graph Editor.";
        ImNodes::BeginNodeEditor();

        // --- Kontextmenü Korrektur ---
        // 1. Hole die Mausklick-Position im Node Editor Space
        m_CurrentMouseGridPosition = GetCurrentMouseGridPosition();

        // 2. ImNodes::IsEditorHovered() ist die robusteste Prüfung.
        //    Trigger the popup BEFORE ImNodes::EndNodeEditor()
        bool editorHovered = ImNodes::IsEditorHovered();
        bool mouseRightReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
        // bool mainWinHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow);
        // bool mainWinFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow);

        // LOG(Info) << "Editor Hovered: " << editorHovered
        //           << ", Mouse Right Released: " << mouseRightReleased
        //           << ", Main Window Hovered: " << mainWinHovered
        //           << ", Main Window Focused: " << mainWinFocused;

        if (editorHovered && mouseRightReleased) {
            LOG(Info) << "Attempting to open NodeEditorContextMenu. editorHovered: " << editorHovered << ", mouseRightReleased: " << mouseRightReleased;
            ImGui::OpenPopup("NodeEditorContextMenu");
        }

        // Draw existing nodes
        for (auto entity : m_Registry.GetEntitiesWith<domain::GlobalParameterComponent>()) {
            if (m_Registry.HasComponent<domain::UINodeComponent>(entity)) {
                DrawParameterNode(entity);
            }
        }

        for (auto entity : m_Registry.GetEntitiesWith<domain::BoxComponent>()) {
            if (m_Registry.HasComponent<domain::UINodeComponent>(entity)) {
                DrawFeatureNode(entity);
            }
        }

        for (auto entity : m_Registry.GetEntitiesWith<domain::CylinderComponent>()) {
            if (m_Registry.HasComponent<domain::UINodeComponent>(entity)) {
                DrawFeatureNode(entity);
            }
        }

        ImNodes::EndNodeEditor();

        // --- Pop-up wird außerhalb von ImNodes::Begin/EndNodeEditor() gezeichnet ---
        if (ImGui::BeginPopup("NodeEditorContextMenu")) {
            // LOG(Info) << "NodeEditorContextMenu IS ACTIVE AND DRAWING!"; // Added log
            if (ImGui::MenuItem("Add Global Parameter")) {
                // LOG(Info) << "Adding Global Parameter";
                m_CommandStack.Push(std::make_unique<nc::cmd::InsertGlobalParameterCommand>(
                    "New Parameter", 1.0, m_CurrentMouseGridPosition));
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Add Box Feature")) {
                // LOG(Info) << "Adding Box Feature";
                m_CommandStack.Push(
                    std::make_unique<nc::cmd::InsertBoxCommand>(10.0, 10.0, 10.0, m_CurrentMouseGridPosition));
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Add Cylinder Feature")) {
                // LOG(Info) << "Adding Cylinder Feature";
                m_CommandStack.Push(
                    std::make_unique<nc::cmd::InsertCylinderCommand>(5.0, 10.0, m_CurrentMouseGridPosition));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    // Wichtig: Schließe das Hauptfenster immer korrekt
    ImGui::End();
}

void GraphEditorSystem::DrawParameterNode(domain::EntityID parameterId) {
    LOG(Info) << "DrawParameterNode called for EntityID: " << parameterId;
    auto* param = m_Registry.GetComponent<domain::GlobalParameterComponent>(parameterId);
    auto* uiNode = m_Registry.GetComponent<domain::UINodeComponent>(parameterId);
    if (!param || !uiNode) {
        LOG(Warn) << "DrawParameterNode: Missing GlobalParameterComponent or UINodeComponent for EntityID: " << parameterId;
        return;
    }

    ImNodes::BeginNode(parameterId);
    ImNodes::SetNodeEditorSpacePos(parameterId, ImVec2(uiNode->positionX, uiNode->positionY));

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", GetNodeTitle(parameterId).c_str());
    ImNodes::EndNodeTitleBar();

    // Output Pin (ID is the parameter's EntityID)
    ImNodes::BeginOutputAttribute(parameterId);
    ImGui::Text("Value");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    // Use UpdateParameterCommand when value changes
    double val = param->value;
    if (ImGui::DragScalar("##value", ImGuiDataType_Double, &val, 0.1f)) {
        m_CommandStack.Push(std::make_unique<nc::cmd::UpdateParameterCommand>(parameterId, val));
    }
    ImGui::PopItemWidth();
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();

    // Update UINodeComponent if node moved
    ImVec2 currentPos = ImNodes::GetNodeEditorSpacePos(parameterId);
    if (currentPos.x != uiNode->positionX || currentPos.y != uiNode->positionY) {
        uiNode->positionX = currentPos.x;
        uiNode->positionY = currentPos.y;
    }
}

void GraphEditorSystem::DrawFeatureNode(domain::EntityID featureId) {
    LOG(Info) << "DrawFeatureNode called for EntityID: " << featureId;
    auto* uiNode = m_Registry.GetComponent<domain::UINodeComponent>(featureId);
    if (!uiNode) {
        LOG(Warn) << "DrawFeatureNode: Missing UINodeComponent for EntityID: " << featureId;
        return;
    }

    ImNodes::BeginNode(featureId);
    ImNodes::SetNodeEditorSpacePos(featureId, ImVec2(uiNode->positionX, uiNode->positionY));

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", GetNodeTitle(featureId).c_str());
    ImNodes::EndNodeTitleBar();

    // Inputs
    if (auto* box = m_Registry.GetComponent<domain::BoxComponent>(featureId)) {
        DrawPinAndInput(box->widthExpressionId, "Width", true);
        DrawPinAndInput(box->lengthExpressionId, "Length", true);
        DrawPinAndInput(box->heightExpressionId, "Height", true);
    } else if (auto* cyl = m_Registry.GetComponent<domain::CylinderComponent>(featureId)) {
        DrawPinAndInput(cyl->radiusExpressionId, "Radius", true);
        DrawPinAndInput(cyl->heightExpressionId, "Height", true);
    }

    ImNodes::EndNode();

    // Update UINodeComponent if node moved
    ImVec2 currentPos = ImNodes::GetNodeEditorSpacePos(featureId);
    if (currentPos.x != uiNode->positionX || currentPos.y != uiNode->positionY) {
        uiNode->positionX = currentPos.x;
        uiNode->positionY = currentPos.y;
    }
}

void GraphEditorSystem::DrawPinAndInput(domain::EntityID expressionId, const std::string& label, bool isInput) {
    auto* expr = m_Registry.GetComponent<domain::ExpressionComponent>(expressionId);
    if (!expr)
        return;

    if (isInput) {
        // Starte das Attribut (den Pin)
        ImNodes::BeginInputAttribute(expressionId);

        // Zeichne das Label
        ImGui::Text("%s", label.c_str());

        // NEU: Nur zeichnen, wenn NICHT verbunden (ansonsten ist das Attribut fuer Links da)
        if (expr->sourceType == domain::ExpressionComponent::SourceType::STATIC_VALUE) {
            // Zeichne das Widget RECHTS neben dem Text
            ImGui::SameLine();
            ImGui::PushItemWidth(60);

            double val = std::get<double>(expr->sourceData);

            // Das Label fuer DragScalar MUSS UNIQUE sein ("##" + ID)
            if (ImGui::DragScalar(("##expr_val_" + std::to_string(expressionId)).c_str(), ImGuiDataType_Double, &val,
                                  0.1f)) {
                // ACHTUNG: UpdateParameterCommand ist fuer GlobalParameterComponent und STATIC_VALUE Expressions
                // unterschiedlich! Hier muss ein Command ausgeführt werden, das die ExpressionComponent direkt ändert.
                // Da wir nur die value-Variable haben, fuehren wir es der Einfachheit halber aus:
                // In einem echten System muesste hier ein UpdateStaticExpressionValueCommand verwendet werden.
                // Fuer den Prototyp nehmen wir an, dass UpdateParameterCommand beide faelle abdeckt.
                m_CommandStack.Push(std::make_unique<nc::cmd::UpdateParameterCommand>(expressionId, val));
            }
            ImGui::PopItemWidth();
        } else {
            // Zeige Link-Status an
            ImGui::SameLine();
            if (std::holds_alternative<domain::EntityID>(expr->sourceData)) {
                domain::EntityID sourceId = std::get<domain::EntityID>(expr->sourceData);
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "[Ref: %s]", GetNodeTitle(sourceId).c_str());
            } else {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "[Ref]");
            }
        }
        // Beende das Attribut
        ImNodes::EndInputAttribute();
    }
    // ... (Ausgabe-Pin Logik bleibt gleich, ist aber nicht interaktiv)
}

std::string GraphEditorSystem::GetNodeTitle(domain::EntityID entityId) {
    if (m_Registry.HasComponent<domain::NameComponent>(entityId)) {
        return m_Registry.GetComponent<domain::NameComponent>(entityId)->name;
    }
    // Fallback:
    return "Unnamed [" + std::to_string(entityId) + "]";
}

glm::vec2 GraphEditorSystem::GetCurrentMouseGridPosition() {
    // ImGui::GetMousePos() returns mouse position in screen space.
    // ImNodes::EditorContextGetPanning() returns the current panning offset.
    // Subtracting the panning from the screen space mouse position gives the mouse position in editor space (grid
    // space).
    ImVec2 mouse_pos_screen = ImGui::GetMousePos();
    ImVec2 editor_panning = ImNodes::EditorContextGetPanning();
    ImVec2 mouse_pos_editor_space = mouse_pos_screen - editor_panning;
    return {mouse_pos_editor_space.x, mouse_pos_editor_space.y};
}

}  // namespace nc::ui
