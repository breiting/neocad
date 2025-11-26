#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <neocad/vis/Material.hpp>
#include <neocad/vis/Shader.hpp>

namespace nc::vis {

class PointSetMaterial : public Material {
   public:
    PointSetMaterial();

    void Apply(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
               std::shared_ptr<Light> light) override;

    std::shared_ptr<Shader> GetShader() const override {
        return m_Shader;
    }

    void SetColor(const glm::vec4& c) {
        m_Color = c;
    }
    void SetRadius(float r) {
        m_Radius = r;
    }

   private:
    std::shared_ptr<Shader> m_Shader;
    glm::vec4 m_Color{1.0f, 0.5f, 0.0f, 1.0f};  // default orange
    float m_Radius = 0.05f;                     // default world-units
};

}  // namespace nc::vis
