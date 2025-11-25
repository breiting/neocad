#include <glad.h>

#include <glm/mat4x4.hpp>
#include <neocad/rendering/Renderer.hpp>

#include "neocad/rendering/PointCloud.hpp"

namespace nc {

void Renderer::BeginFrame(const ICamera& cam) {
    // Store camera matrices for this frame
    m_View = cam.View();
    m_Proj = cam.Projection();

    // Basic GL state for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // (Optional) leave clear() dem Window überlassen:
    // Window::BeginFrame() sollte glClearColor + glClear machen.
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

}  // namespace nc
