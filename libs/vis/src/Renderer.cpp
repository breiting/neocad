#include <glad.h>

#include <glm/mat4x4.hpp>
#include <neocad/vis/PointCloud.hpp>
#include <neocad/vis/Renderer.hpp>

namespace nc::vis {

void Renderer::BeginFrame(const glm::mat4& view, const glm::mat4& proj) {
    m_View = view;
    m_Proj = proj;

    // Basic GL state for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Renderer::DrawMesh(const Mesh& mesh, const glm::mat4& model) {
    if (!m_Material) {
        // No material bound → nothing to draw
        return;
    }

    // Ask material to bind shader & set uniforms (proj/view/model, colors, etc.)
    m_Material->Apply(m_Proj, m_View, model, m_Light);

    // Delegate geometry draw to  wrapper
    mesh.Render();
}

void Renderer::DrawLineSet(const LineSet& lines, const glm::mat4& model) {
    if (!m_Material) {
        return;
    }

    m_Material->Apply(m_Proj, m_View, model, m_Light);
    lines.Render();
}

void Renderer::DrawPoints(const PointCloud& points, const glm::mat4& model) {
    if (!m_Material) {
        return;
    }

    m_Material->Apply(m_Proj, m_View, model, m_Light);
    points.Render();
}

void Renderer::EndFrame() {
    // Currently nothing to do here.
    // Could be extended later (e.g. debug overlays,  timers, etc.)
    // glFlush();  // usually not needed when using double buffering
}

}  // namespace nc::vis
