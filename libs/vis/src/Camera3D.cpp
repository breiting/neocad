#include <glm/gtc/matrix_transform.hpp>
#include <neocad/vis/Camera3D.hpp>

namespace nc::vis {

void Camera3D::SetTarget(const glm::vec3& t) {
    m_Target = t;
}

void Camera3D::SetDistance(float d) {
    m_Distance = glm::max(d, 0.1f);
}

void Camera3D::AddYawPitch(float dYaw, float dPitch) {
    m_Yaw += dYaw;
    m_Pitch += dPitch;
    m_Pitch = glm::clamp(m_Pitch, -1.5f, 1.5f);  // nicht flippen
}

void Camera3D::Dolly(float delta) {
    m_Distance = glm::max(0.1f, m_Distance - delta);
}

void Camera3D::Pan(float dx, float dy, float aspect) {
    glm::vec3 fwd = glm::normalize(m_Target - GetPosition());
    glm::vec3 right = glm::normalize(glm::cross(fwd, {0, 1, 0}));
    glm::vec3 up = glm::cross(right, fwd);

    m_Target += (-right * dx * aspect + up * dy);
}

glm::vec3 Camera3D::GetPosition() const {
    float cx = cos(m_Yaw) * cos(m_Pitch);
    float cy = sin(m_Pitch);
    float cz = sin(m_Yaw) * cos(m_Pitch);

    glm::vec3 dir = glm::normalize(glm::vec3(cx, cy, cz));
    return m_Target + dir * m_Distance;
}

void Camera3D::SetAspect(float aspect) {
    m_Aspect = aspect;
}

glm::mat4 Camera3D::View() const {
    return glm::lookAt(GetPosition(), m_Target, glm::vec3(0, 0, 1));
}
glm::mat4 Camera3D::Projection() const {
    return glm::perspective(glm::radians(m_FovY), m_Aspect, m_Near, m_Far);
}

}  // namespace nc::vis
