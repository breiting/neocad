#pragma once
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ITool.hpp>
#include <vector>

namespace nc::editor {

enum class CurveMode {
    Line,
    Polyline,
    Face
};

class SketchCurveTool : public ITool {
   public:
    SketchCurveTool() = default;
    explicit SketchCurveTool(CurveMode mode) : m_Mode(mode) {
    }

    // --- Interface from ITool ---
    void OnEnter(ToolContext& ctx) override;
    void OnExit(ToolContext& ctx) override;
    void OnInput(const InputEvent& ev, ToolContext& ctx) override;

   private:
    void CreateLine(domain::Entity a, domain::Entity b, domain::Registry& reg);
    void FinalizePolyline(ToolContext& ctx);
    void FinalizeFace(ToolContext& ctx);

   private:
    CurveMode m_Mode = CurveMode::Line;

    std::vector<domain::Entity> m_Points;
    bool m_WaitingSecondPoint = false;

    // --- Live Preview ---
    domain::Entity m_PreviewPoint = domain::INVALID_ENTITY;     // "hovering" point
    domain::Entity m_LastPreviewLine = domain::INVALID_ENTITY;  // rubberband
};

}  // namespace nc::editor
