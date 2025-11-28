#include <imgui.h>
#include <imnodes.h>

#include <neocad/domain/Components.hpp>
#include <neocad/ui/GraphEditorSystem.hpp>

namespace nc::ui {

GraphEditorSystem::GraphEditorSystem(domain::Registry& registry) : m_Registry(registry) {
}

void GraphEditorSystem::ToggleVisibility() {
    m_IsVisible = !m_IsVisible;
}

void GraphEditorSystem::DrawPanel() {
    if (!m_IsVisible)
        return;

    if (ImGui::Begin("NeoCAD Graph Editor", &m_IsVisible)) {
        ImNodes::BeginNodeEditor();

        // Draw Global Parameters
        auto params = m_Registry.GetEntitiesWith<domain::GlobalParameterComponent>();
        for (auto entity : params) {
            DrawParameterNode(entity);
        }

        // Draw Box Features
        auto boxes = m_Registry.GetEntitiesWith<domain::BoxComponent>();
        for (auto entity : boxes) {
            DrawFeatureNode(entity);
        }

        // Draw Cylinder Features
        auto cylinders = m_Registry.GetEntitiesWith<domain::CylinderComponent>();
        for (auto entity : cylinders) {
            DrawFeatureNode(entity);
        }

        // Draw Links
        // In this simple demo, we iterate over ExpressionComponents of features
        // and check if they reference a GlobalParameter.
        // Link ID strategy: We need unique IDs for links.
        // ImNodes requires int IDs. EntityID is uint32_t.
        // We can construct a link ID. For simplicity in this demo, we won't persist links
        // properly but just draw them if we find the relationship.
        // A better way is to have Link entities.
        // Here, we just iterate expressions and if they are references, we draw a link.

        int linkIdCounter = 10000;  // Offset to avoid collision with node/pin IDs

        auto drawLink = [&](domain::EntityID sourceId, domain::EntityID targetPinId) {
            ImNodes::Link(linkIdCounter++, sourceId, targetPinId);
            // Note: ImNodes::Link takes (link_id, start_attribute_id, end_attribute_id)
            // Our "Global Parameter" output pin ID is the EntityID of the parameter.
            // Our "Feature Expression" input pin ID is the EntityID of the expression.
        };

        auto expressions = m_Registry.GetEntitiesWith<domain::ExpressionComponent>();
        for (auto exprId : expressions) {
            auto* expr = m_Registry.GetComponent<domain::ExpressionComponent>(exprId);
            if (expr && expr->sourceType == domain::ExpressionComponent::SourceType::ENTITY_REFERENCE) {
                if (std::holds_alternative<domain::EntityID>(expr->sourceData)) {
                    domain::EntityID targetId = std::get<domain::EntityID>(expr->sourceData);
                    // Target is a GlobalParameter. Use its EntityID as the Output Pin ID.
                    // The Expression EntityID is the Input Pin ID.
                    drawLink(targetId, exprId);
                }
            }
        }

        ImNodes::EndNodeEditor();
    }
    ImGui::End();
}

void GraphEditorSystem::DrawParameterNode(domain::EntityID parameterId) {
    auto* param = m_Registry.GetComponent<domain::GlobalParameterComponent>(parameterId);
    if (!param)
        return;

    ImNodes::BeginNode(parameterId);

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", param->name.c_str());
    ImNodes::EndNodeTitleBar();

    // Output Pin
    ImNodes::BeginOutputAttribute(parameterId);
    ImGui::Text("Value");
    ImNodes::EndOutputAttribute();

    // Editable Value
    ImGui::PushItemWidth(100);
    if (ImGui::DragScalar("##value", ImGuiDataType_Double, &param->value, 0.1f)) {
        param->version++;
    }
    ImGui::PopItemWidth();

    ImNodes::EndNode();

    // Update position if moved (Demo logic: assuming one-way sync for now or usage of UINodeComponent)
    // Ideally, we read UINodeComponent to set position, and write back.
    auto* uiComp = m_Registry.GetComponent<domain::UINodeComponent>(parameterId);
    if (uiComp) {
        // Set position only once or if it changed externally?
        // ImNodes keeps internal state. We might need ImNodes::SetNodeGridSpacePos(id, pos)
        // if we want to restore.
        // For this task, we just ensured the component exists.
    }
}

void GraphEditorSystem::DrawFeatureNode(domain::EntityID featureId) {
    auto* nameComp = m_Registry.GetComponent<domain::NameComponent>(featureId);
    std::string title = nameComp ? nameComp->name : "Feature " + std::to_string(featureId);

    ImNodes::BeginNode(featureId);

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", title.c_str());
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
}

void GraphEditorSystem::DrawPinAndInput(domain::EntityID expressionId, const std::string& label, bool isInput) {
    auto* expr = m_Registry.GetComponent<domain::ExpressionComponent>(expressionId);
    if (!expr)
        return;

    if (isInput) {
        ImNodes::BeginInputAttribute(expressionId);
        ImGui::Text("%s", label.c_str());
        ImNodes::EndInputAttribute();
    } else {
        ImNodes::BeginOutputAttribute(expressionId);
        ImGui::Text("%s", label.c_str());
        ImNodes::EndOutputAttribute();
    }

    // Show value or reference info
    if (expr->sourceType == domain::ExpressionComponent::SourceType::STATIC_VALUE) {
        ImGui::SameLine();
        ImGui::PushItemWidth(60);
        if (std::holds_alternative<double>(expr->sourceData)) {
            double& val = std::get<double>(expr->sourceData);
            if (ImGui::DragScalar(("##" + std::to_string(expressionId)).c_str(), ImGuiDataType_Double, &val, 0.1f)) {
                expr->evaluatedValue = val;
                expr->version++;
            }
        }
        ImGui::PopItemWidth();
    } else {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "[Ref]");
    }
}

}  // namespace nc::ui
