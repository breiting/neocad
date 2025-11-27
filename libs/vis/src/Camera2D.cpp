#include <algorithm>
#include <neocad/vis/Camera2D.hpp>

namespace nc::vis {

Camera2D::Camera2D() : m_Position(0.0f), m_FirstMouse(true), m_Zoom(5.0f), m_AspectRatio(1.0f) {
}

void Camera2D::OnMouseStart() {
    m_FirstMouse = true;
}

void Camera2D::OnMousePan(double xpos, double ypos) {
    if (m_FirstMouse) {
        m_LastMouse = {xpos, ypos};
        m_FirstMouse = false;
        return;
    }

    glm::vec2 newMouse(xpos, ypos);
    glm::vec2 delta = newMouse - m_LastMouse;
    m_LastMouse = newMouse;

    float speed = m_Zoom * 0.002f;  // Pan beschleunigt bei Zoom
    m_Position.x -= delta.x * speed;
    m_Position.y += delta.y * speed;
}

void Camera2D::OnMouseScroll(double yoffset) {
    m_Zoom -= yoffset * 0.5f;
    m_Zoom = std::max(0.5f, m_Zoom);  // Mindest-Zoom
}

glm::mat4 Camera2D::GetViewMatrix() const {
    return glm::lookAt(glm::vec3(m_Position.x, m_Position.y, 10.0f),  // Position
                       glm::vec3(m_Position.x, m_Position.y, 0.0f),   // Target
                       glm::vec3(0, 1, 0)                             // Up
    );
}

glm::mat4 Camera2D::GetProjectionMatrix() const {
    float half = m_Zoom;
    return glm::ortho(-half * m_AspectRatio, +half * m_AspectRatio, -half, +half, -100.0f, +100.0f);
}

void Camera2D::SetPosition(const glm::vec2& pos) {
    m_Position = pos;
}

glm::vec3 Camera2D::ScreenToWorld(double x, double y) const {
    float ndcX = (2.0f * x / m_VP.x) - 1.0f;
    float ndcY = -(2.0f * y / m_VP.y) + 1.0f;  // Y-Achse invertieren!

    glm::vec4 ndc(ndcX, ndcY, 0.0f, 1.0f);

    glm::mat4 inv = glm::inverse(GetProjectionMatrix() * GetViewMatrix());
    glm::vec4 world = inv * ndc;

    return glm::vec3(world) / world.w;
}

}  // namespace nc::vis
