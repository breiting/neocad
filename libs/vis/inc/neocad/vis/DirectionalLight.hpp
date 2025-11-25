#pragma once
#include <glm/glm.hpp>
#include <neocad/vis/Light.hpp>

namespace nc::vis {

class DirectionalLight : public Light {
   public:
    void SetColor(const glm::vec3 &color);
    void SetDirection(const glm::vec3 &dir);

    virtual glm::vec3 GetColor() const override;
    virtual glm::vec3 GetDirection() const override;
    virtual glm::vec3 GetPosition() const override;

   private:
    glm::vec3 m_Direction;
    glm::vec3 m_Color;
};
}  // namespace nc::vis
