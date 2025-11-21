#include "nc/rendering/SceneRenderer2D.hpp"

namespace nc::rendering {

using namespace nc::core;

void SceneRenderer2D::RenderSketch2D(const SketchRegistry& registry) {
    // 1) Draw lines
    for (const auto& [lineId, line] : registry.Lines2D()) {
        const Point3DComponent* p0 = registry.GetPoint3D(line.p0);
        const Point3DComponent* p1 = registry.GetPoint3D(line.p1);
        if (!p0 || !p1) continue;

        glm::vec2 a(p0->position.x, p0->position.y);
        glm::vec2 b(p1->position.x, p1->position.y);

        m_Canvas.DrawLine(a, b);
    }

    // 2) Draw points (optional: on top)
    for (const auto& [pointId, pt] : registry.Points3D()) {
        glm::vec2 p(pt.position.x, pt.position.y);
        m_Canvas.DrawPoint(p);
    }

    // Faces2D we könnten später füllen/rendern, aber fürs MVP
    // reicht Linien + Punkte als visuelles Feedback.
}

}  // namespace nc::rendering
