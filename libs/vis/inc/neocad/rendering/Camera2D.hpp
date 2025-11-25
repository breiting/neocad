#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <neocad/rendering/ICamera.hpp>

namespace nc::vis {

class Camera2D : public ICamera {
   public:
    void SetCenter(const glm::vec2& c) {
        m_Center = c;
    }
    void Pan(float dx, float dy);
    void Zoom(float factor);  // >1 zoom in, <1 zoom out
                              //
    void SetAspect(float aspect) override;
    glm::mat4 View() const override;
    glm::mat4 Projection() const override;

   private:
    glm::vec2 m_Center{0.0, 0.0};
    float m_Scale{1.0};  // 1.0 = 1 Welt-Einheit == 1 NDC-Einheit (togglebar)
    float m_Near{-1.0};
    float m_Far{+1.0};
    float m_Aspect{1.0};
};

}  // namespace nc
