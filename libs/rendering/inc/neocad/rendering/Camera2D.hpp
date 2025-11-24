#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace nc {

class Camera2D {
   public:
    void SetCenter(const glm::dvec2& c) {
        m_Center = c;
    }
    void Pan(double dx, double dy);
    void Zoom(double factor);  // >1 zoom in, <1 zoom out

    glm::dmat4 GetViewMatrix() const;
    glm::dmat4 GetProjMatrix(double aspect) const;

   private:
    glm::dvec2 m_Center{0.0, 0.0};
    double m_Scale{1.0};  // 1.0 = 1 Welt-Einheit == 1 NDC-Einheit (togglebar)
    double m_Near{-1.0};
    double m_Far{+1.0};
};

}  // namespace nc
