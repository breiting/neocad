#include <glm/gtc/matrix_transform.hpp>
#include <neocad/rendering/Camera2D.hpp>

namespace nc::vis {

void Camera2D::Pan(float dx, float dy) {
    m_Center += glm::vec2(dx, dy) * m_Scale;
}

void Camera2D::Zoom(float factor) {
    m_Scale /= factor;
}

void Camera2D::SetAspect(float aspect) {
    m_Aspect = aspect;
}

glm::mat4 Camera2D::View() const {
    float halfW = m_Scale;
    float halfH = m_Scale / m_Aspect;
    return glm::ortho(-halfW, halfW, -halfH, halfH, m_Near, m_Far);
}

glm::mat4 Camera2D::Projection() const {
    glm::vec3 eye = {m_Center.x, m_Center.y, +10.0};
    glm::vec3 at = {m_Center.x, m_Center.y, 0.0};
    return glm::lookAt(eye, at, glm::vec3(0, 1, 0));
}

}  // namespace nc::vis
