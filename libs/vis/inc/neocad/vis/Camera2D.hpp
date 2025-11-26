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
    void OnMouseRotation(double xpos, double ypos) override {
    }  // No rotation in 2D
    void OnMousePan(double xpos, double ypos) override;
    void OnMouseScroll(double yoffset) override;
    void Update(float /*dt*/) override {
    }

    glm::mat4 GetViewMatrix() const override;
    glm::mat4 GetProjectionMatrix() const override;
    void SetAspectRatio(float aspect) override;
    glm::vec3 GetViewDirection() const override {
        return {0, 0, -1};
    }

    // Helpers
    void SetPosition(const glm::vec2& pos);
    const glm::vec2& GetPosition() const {
        return m_Position;
    }
    float GetZoom() const {
        return m_Zoom;
    }

   private:
    glm::vec2 m_Position;
    glm::vec2 m_LastMouse;
    bool m_FirstMouse;
    float m_Zoom;
    float m_AspectRatio;
};

}  // namespace nc::vis
