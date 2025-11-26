#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <neocad/editor/ICamera.hpp>

namespace nc::vis {

class Camera2D : public editor::ICamera {
   public:
    Camera2D();

    // --- ICamera Interface ---
    void OnMouseStart() override;
    void OnMouseRotation(double xpos, double ypos) override;  // ignored for 2D
    void OnMousePan(double xpos, double ypos) override;
    void OnMouseScroll(double yoffset) override;
    void Update(float /*dt*/) override {
    }

    glm::mat4 GetViewMatrix() const override;
    glm::mat4 GetProjectionMatrix() const override;
    void SetAspectRatio(float aspect) override;
    // --------------------------

    // Optional helpers
    void SetPosition(const glm::vec2& pos);
    const glm::vec2& GetPosition() const;
    float GetZoom() const;

   private:
    glm::vec2 m_Position;   // center in world units
    glm::vec2 m_LastMouse;  // pixel coords
    bool m_FirstMouse;
    float m_Zoom;         // scale factor
    float m_AspectRatio;  // W/H ratio
};

}  // namespace nc::vis
