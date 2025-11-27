#pragma once
#include <chrono>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ITool.hpp>
#include <neocad/editor/Mode.hpp>
#include <vector>

namespace nc::editor {

class SketchCurveTool : public ITool {
   public:
    enum class CurveMode {
        Line,
        Polyline,
        Face
    };

    SketchCurveTool(CurveMode mode) : m_Mode(mode) {
    }

    void OnEnter(ToolContext& ctx) override;
    void OnExit(ToolContext& ctx) override;
    bool OnInput(const InputEvent& ev, ToolContext& ctx) override;

   private:
    CurveMode m_Mode;
    std::vector<nc::domain::Entity> m_Points;

    nc::domain::Entity m_PreviewPoint = domain::INVALID_ENTITY;
    nc::domain::Entity m_LastPreviewLine = domain::INVALID_ENTITY;
    bool m_WaitingSecondPoint = false;
    
    size_t m_StartCmdIndex{0};

    using Clock = std::chrono::steady_clock;
    Clock::time_point m_LastClickTime = Clock::now();

    void CreateLine(nc::domain::Entity a, nc::domain::Entity b, domain::Registry& reg);
    void FinalizePolyline(ToolContext& ctx);
    void FinalizeFace(ToolContext& ctx);
};

}  // namespace nc::editor
