#include <glm/gtc/matrix_transform.hpp>
#include <neocad/rendering/Camera2D.hpp>

namespace nc {

void Camera2D::Pan(double dx, double dy) {
    m_Center += glm::dvec2(dx, dy) * m_Scale;
}

void Camera2D::Zoom(double factor) {
    m_Scale /= factor;
}

glm::dmat4 Camera2D::GetViewMatrix() const {
    glm::dvec3 eye = {m_Center.x, m_Center.y, +10.0};
    glm::dvec3 at = {m_Center.x, m_Center.y, 0.0};
    return glm::lookAt(eye, at, glm::dvec3(0, 1, 0));
}

glm::dmat4 Camera2D::GetProjMatrix(double aspect) const {
    double halfW = m_Scale;
    double halfH = m_Scale / aspect;
    return glm::ortho(-halfW, halfW, -halfH, halfH, m_Near, m_Far);
}

}  // namespace nc
