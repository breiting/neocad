#pragma once
#include <glm/glm.hpp>
#include <neocad/editor/ICamera.hpp>

namespace nc::vis {

class Camera3D : public editor::ICamera {
   public:
    Camera3D(float radius = 10.0f, float pitch = 45.0f, float yaw = 45.0f);

    void OnMouseStart() override;
    void OnMouseRotation(double xpos, double ypos) override;
    void OnMousePan(double xpos, double ypos) override;
    void OnMouseScroll(double yoffset) override;
    void Update(float deltaTime) override;

    glm::mat4 GetViewMatrix() const override;
    glm::mat4 GetProjectionMatrix() const override;

    glm::vec3 GetViewDirection() const override;
    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetTarget() const;

    glm::vec3 ScreenToWorld(double screenX, double screenY) const override;

    void SetViewport(int w, int h) override {
        m_VP = {float(w), float(h)};
        m_AspectRatio = float(w) / float(h);
    }

   private:
    glm::vec2 OnMouseMove(double xpos, double ypos);
    void UpdatePosition();

    glm::vec2 m_Rotation;
    glm::vec2 m_RotationVelocity;
    glm::vec2 m_VP;
    float m_Radius;
    float m_DampingFactor;
    float m_VelocityThreshold;
    glm::vec2 m_LastMouse;
    bool m_FirstMouse;

    glm::vec3 m_Position{0.0f, 0.0f, 5.0f};
    glm::vec3 m_Target{0.0f, 0.0f, 0.0f};
    float m_AspectRatio{1.0f};
};

}  // namespace nc::vis
