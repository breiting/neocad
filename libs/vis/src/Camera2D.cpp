#include <algorithm>
#include <neocad/vis/Camera2D.hpp>

namespace nc::vis {

Camera2D::Camera2D()
    : m_Position(0.0f, 0.0f), m_LastMouse(0.0f), m_FirstMouse(true), m_Zoom(1.0f), m_AspectRatio(1.0f) {
}

void Camera2D::SetAspectRatio(float aspect) {
    m_AspectRatio = aspect;
}

void Camera2D::SetPosition(const glm::vec2& pos) {
    m_Position = pos;
}

const glm::vec2& Camera2D::GetPosition() const {
    return m_Position;
}

float Camera2D::GetZoom() const {
    return m_Zoom;
}

void Camera2D::OnMouseStart() {
    m_FirstMouse = true;
}

void Camera2D::OnMouseRotation(double /*xpos*/, double /*ypos*/) {
    // Intentionally ignored – no rotation in 2D mode
}

void Camera2D::OnMousePan(double xpos, double ypos) {
    if (m_FirstMouse) {
        m_LastMouse = {xpos, ypos};
        m_FirstMouse = false;
        return;
    }

    glm::vec2 cur(xpos, ypos);
    glm::vec2 delta = cur - m_LastMouse;
    m_LastMouse = cur;

    // Screen px → world units
    float panSpeed = 1.0f / m_Zoom;  // Zoom-in = slow pan
    m_Position.x -= delta.x * 0.001f * panSpeed;
    m_Position.y += delta.y * 0.001f * panSpeed;
}

void Camera2D::OnMouseScroll(double yoffset) {
    // Scale factor
    float zoomFactor = std::exp(yoffset * 0.2f);
    m_Zoom = std::clamp(m_Zoom * zoomFactor, 0.05f, 50.0f);
}

glm::mat4 Camera2D::GetViewMatrix() const {
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(-m_Position, 0.0f));
    view = glm::scale(view, glm::vec3(m_Zoom, m_Zoom, 1.0f));
    return view;
}

glm::mat4 Camera2D::GetProjectionMatrix() const {
    // Symmetrical ortho projection based on aspect + zoom
    float orthoSize = 1.0f;  // world space extent
    float halfW = orthoSize * m_AspectRatio;
    float halfH = orthoSize;
    return glm::ortho(-halfW * m_Zoom, +halfW * m_Zoom, -halfH * m_Zoom, +halfH * m_Zoom, -10.0f, +10.0f);
}

}  // namespace nc::vis
