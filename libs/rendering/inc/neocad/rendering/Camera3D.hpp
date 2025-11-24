#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace nc {

class Camera3D {
   public:
    void SetTarget(const glm::dvec3& t);
    void SetDistance(double d);
    void AddYawPitch(double dYaw, double dPitch);
    void Dolly(double delta);  // Scroll-Zoom
    void Pan(double dx, double dy, double aspect);

    glm::dmat4 GetViewMatrix() const;
    glm::dmat4 GetProjMatrix(double aspect) const;
    glm::dvec3 GetPosition() const;

   private:
    glm::dvec3 m_Target{0.0, 0.0, 0.0};
    double m_Distance{6.0};
    double m_Yaw{0.0};
    double m_Pitch{0.35};  // + leicht nach unten

    double m_FovY{45.0};
    double m_Near{0.01};
    double m_Far{1000.0};
};

}  // namespace nc
