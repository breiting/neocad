#include <glad.h>

#include <glm/mat4x4.hpp>
#include <memory>
#include <neocad/core/Colors.hpp>
#include <neocad/core/Logger.hpp>
#include <neocad/vis/DirectionalLight.hpp>
#include <neocad/vis/OpenGLRenderer.hpp>
#include <neocad/vis/PointCloud.hpp>

namespace nc::vis {

OpenGLRenderer::OpenGLRenderer() : m_Wireframe(false) {
    m_Light = std::make_shared<DirectionalLight>();

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glDepthFunc(GL_LESS);

    glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

OpenGLRenderer::~OpenGLRenderer() {
    glDisable(GL_DEPTH_TEST);
}

void OpenGLRenderer::SetViewportSize(int w, int h) {
    LOG(INFO) << "Setting viewport to " << w << "x" << h;
    glViewport(0, 0, w, h);
}

void OpenGLRenderer::ToggleWireframe() {
    m_Wireframe = !m_Wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);
}

void OpenGLRenderer::BeginFrame(const glm::mat4& view, const glm::mat4& proj) {
    m_View = view;
    m_Proj = proj;

    auto bg = core::Nord0;
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::DrawMesh(const std::shared_ptr<BaseGeometry> mesh, std::shared_ptr<Material> mat,
                              const glm::mat4& model) {
    mat->Apply(m_Proj, m_View, model, m_Light);
    mesh->Render();
}

void OpenGLRenderer::DrawLineSet(const std::shared_ptr<BaseGeometry> lines, std::shared_ptr<Material> mat,
                                 const glm::mat4& model) {
    mat->Apply(m_Proj, m_View, model, m_Light);
    lines->Render();
}

void OpenGLRenderer::DrawPoints(const std::shared_ptr<BaseGeometry> points, std::shared_ptr<Material> mat,
                                const glm::mat4& model) {
    mat->Apply(m_Proj, m_View, model, m_Light);
    points->Render();
}

void OpenGLRenderer::EndFrame() {
}

}  // namespace nc::vis
