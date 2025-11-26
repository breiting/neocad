#include <glad.h>

#include <glm/mat4x4.hpp>
#include <memory>
#include <neocad/vis/DirectionalLight.hpp>
#include <neocad/vis/PointCloud.hpp>
#include <neocad/vis/Renderer.hpp>

namespace nc::vis {

Renderer::Renderer() {
    m_Light = std::make_shared<DirectionalLight>();
}

void Renderer::BeginFrame(const glm::mat4& view, const glm::mat4& proj) {
    m_View = view;
    m_Proj = proj;

    // Basic GL state for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::DrawMesh(const Mesh& mesh, std::shared_ptr<Material> mat, const glm::mat4& model) {
    mat->Apply(m_Proj, m_View, model, m_Light);
    mesh.Render();
}

void Renderer::DrawLineSet(const LineSet& lines, std::shared_ptr<Material> mat, const glm::mat4& model) {
    mat->Apply(m_Proj, m_View, model, m_Light);
    lines.Render();
}

void Renderer::DrawPoints(const PointCloud& points, std::shared_ptr<Material> mat, const glm::mat4& model) {
    mat->Apply(m_Proj, m_View, model, m_Light);
    points.Render();
}

void Renderer::EndFrame() {
    // Currently nothing to do here.
    // Could be extended later (e.g. debug overlays,  timers, etc.)
    // glFlush();  // usually not needed when using double buffering
}

}  // namespace nc::vis
