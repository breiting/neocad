#pragma once
#include <glm/vec2.hpp>

#include "nc/core/SketchRegistry.hpp"
#include "nc/rendering/CanvasRenderer2D.hpp"

namespace nc::rendering {

/// Bridges SketchRegistry (domain) to CanvasRenderer2D (rendering).
class SceneRenderer2D {
   public:
    explicit SceneRenderer2D(CanvasRenderer2D& canvas) : m_Canvas(canvas) {
    }

    /// Draw all sketch entities (points + lines) in 2D.
    /// Z-component of points is ignored for screen-space.
    void RenderSketch2D(const nc::core::SketchRegistry& registry);

   private:
    CanvasRenderer2D& m_Canvas;
};

}  // namespace nc::rendering
