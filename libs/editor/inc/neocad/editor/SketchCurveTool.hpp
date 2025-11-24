#pragma once
#include <neocad/domain/Components.hpp>
#include <neocad/domain/Entity.hpp>
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ITool.hpp>
#include <vector>

namespace nc {

enum class CurveMode {
    Line,      // 2 points, immediate line
    Polyline,  // ESC ends, open curve
    Face       // ENTER closes & makes FaceComponent
};

class SketchCurveTool : public ITool {
   public:
    explicit SketchCurveTool(CurveMode mode) : m_Mode(mode) {
    }

    void OnEnter(ToolContext&) override;
    void OnExit(ToolContext&) override;
    void OnInput(const InputEvent&, ToolContext&) override;

   private:
    CurveMode m_Mode;
    std::vector<Entity> m_Points;       // captured dynamic points
    bool m_WaitingSecondPoint = false;  // only relevant in Line mode

    void CreateLine(Entity a, Entity b, Registry&);
    void FinalizePolyline(ToolContext&);
    void FinalizeFace(ToolContext&);
};

}  // namespace nc
