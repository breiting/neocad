#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>  // glm::perspective
#include <glm/ext/matrix_transform.hpp>   // glm::lookat
#include <glm/glm.hpp>
#include <neocad/vis/Camera3D.hpp>

namespace nc::vis {

Camera3D::Camera3D(float radius, float pitch, float yaw)
    : m_Rotation({yaw, pitch}),
      m_RotationVelocity(0.0f),
      m_Radius(radius),
      m_DampingFactor(0.90f),
      m_VelocityThreshold(0.001f),
      m_LastMouse(0.0f),
      m_FirstMouse(true) {
    UpdatePosition();
}

void Camera3D::OnMouseStart() {
    m_FirstMouse = true;
}

glm::vec2 Camera3D::OnMouseMove(double xpos, double ypos) {
    if (m_FirstMouse) {
        m_LastMouse = glm::vec2(xpos, ypos);
        m_FirstMouse = false;
    }

    glm::vec2 currentMouse(xpos, ypos);
    glm::vec2 delta = currentMouse - m_LastMouse;
    m_LastMouse = currentMouse;

    float sensitivity = 2.0f;
    delta *= sensitivity;
    return delta;
}

void Camera3D::OnMouseRotation(double xpos, double ypos) {
    auto delta = OnMouseMove(xpos, ypos);
    float speed = 1.0f;
    m_RotationVelocity += delta * speed;
    UpdatePosition();
}

void Camera3D::OnMousePan(double xpos, double ypos) {
    auto delta = OnMouseMove(xpos, ypos);
    glm::vec3 right = glm::normalize(glm::cross(GetViewDirection(), glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, GetViewDirection()));

    m_Target -= right * delta.x * 0.001f;
    m_Target += up * delta.y * 0.001f;
    UpdatePosition();
}

void Camera3D::OnMouseScroll(double yoffset) {
    float factor = static_cast<float>(yoffset);
    float zoomSpeed = 0.2f;
    factor = copysignf(powf(std::abs(factor), 1.2f), factor);  // nichtlinear
    m_Radius -= factor * zoomSpeed;
    m_Radius = std::clamp(m_Radius, 1.0f, 100.0f);
    UpdatePosition();
}

void Camera3D::Update(float deltaTime) {
    m_Rotation += m_RotationVelocity * deltaTime;
    m_Rotation.y = std::clamp(m_Rotation.y, -89.0f, 89.0f);
    m_RotationVelocity *= m_DampingFactor;
    if (glm::length(m_RotationVelocity) < m_VelocityThreshold) {
        m_RotationVelocity = glm::vec2(0.0f);
    }
    UpdatePosition();
}

void Camera3D::UpdatePosition() {
    float radPitch = glm::radians(m_Rotation.y);
    float radYaw = glm::radians(m_Rotation.x);

    glm::vec3 offset;
    offset.x = m_Radius * cos(radPitch) * cos(radYaw);
    offset.y = m_Radius * sin(radPitch);
    offset.z = m_Radius * cos(radPitch) * sin(radYaw);

    m_Position = m_Target + offset;
}

void Camera3D::SetPosition(const glm::vec3& position) {
    m_Position = position;
}

void Camera3D::SetTarget(const glm::vec3& target) {
    m_Target = target;
}

const glm::vec3& Camera3D::GetPosition() const {
    return m_Position;
}

const glm::vec3& Camera3D::GetTarget() const {
    return m_Target;
}

glm::mat4 Camera3D::GetViewMatrix() const {
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::lookAt(m_Position, m_Target, up);
}

glm::mat4 Camera3D::GetProjectionMatrix() const {
    // TODO: FOV should be dynamic
    float fov = 45.0;
    return glm::perspective(glm::radians(fov), m_AspectRatio, 0.01f, 500.0f);
}

void Camera3D::SetAspectRatio(float aspect) {
    m_AspectRatio = aspect;
}

glm::vec3 Camera3D::GetViewDirection() const {
    return glm::normalize(m_Target - m_Position);
}

}  // namespace nc::vis
