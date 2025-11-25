#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <neocad/rendering/ICamera.hpp>

namespace nc {

class Camera3D : public ICamera {
   public:
    void SetTarget(const glm::vec3& t);
    void SetDistance(float d);
    void AddYawPitch(float dYaw, float dPitch);
    void Dolly(float delta);  // Scroll-Zoom
    void Pan(float dx, float dy, float aspect);

    void SetAspect(float aspect) override;
    glm::mat4 View() const override;
    glm::mat4 Projection() const override;

    glm::vec3 GetPosition() const;

   private:
    glm::vec3 m_Target{0.0, 0.0, 0.0};
    float m_Distance{6.0};
    float m_Yaw{0.0};
    float m_Pitch{0.35};  // + leicht nach unten

    float m_FovY{45.0};
    float m_Near{0.01};
    float m_Far{1000.0};
    float m_Aspect{1.0};
};

}  // namespace nc
