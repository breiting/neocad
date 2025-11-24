#include <glm/gtc/matrix_transform.hpp>
#include <neocad/rendering/Camera3D.hpp>

namespace nc {

void Camera3D::SetTarget(const glm::dvec3& t) {
    m_Target = t;
}

void Camera3D::SetDistance(double d) {
    m_Distance = glm::max(d, 0.1);
}

void Camera3D::AddYawPitch(double dYaw, double dPitch) {
    m_Yaw += dYaw;
    m_Pitch += dPitch;
    m_Pitch = glm::clamp(m_Pitch, -1.5, 1.5);  // nicht flippen
}

void Camera3D::Dolly(double delta) {
    m_Distance = glm::max(0.1, m_Distance - delta);
}

void Camera3D::Pan(double dx, double dy, double aspect) {
    glm::dvec3 fwd = glm::normalize(m_Target - GetPosition());
    glm::dvec3 right = glm::normalize(glm::cross(fwd, {0, 1, 0}));
    glm::dvec3 up = glm::cross(right, fwd);

    m_Target += (-right * dx * aspect + up * dy);
}

glm::dvec3 Camera3D::GetPosition() const {
    double cx = cos(m_Yaw) * cos(m_Pitch);
    double cy = sin(m_Pitch);
    double cz = sin(m_Yaw) * cos(m_Pitch);

    glm::dvec3 dir = glm::normalize(glm::dvec3(cx, cy, cz));
    return m_Target + dir * m_Distance;
}

glm::dmat4 Camera3D::GetViewMatrix() const {
    return glm::lookAt(GetPosition(), m_Target, glm::dvec3(0, 0, 1));
}

glm::dmat4 Camera3D::GetProjMatrix(double aspect) const {
    return glm::perspective(glm::radians(m_FovY), aspect, m_Near, m_Far);
}

}  // namespace nc
