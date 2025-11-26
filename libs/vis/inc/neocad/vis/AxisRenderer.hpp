#pragma once
#include <glad.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <neocad/vis/Shader.hpp>

namespace nc::vis {

class AxisRenderer {
   public:
    AxisRenderer() = default;
    ~AxisRenderer();

    bool Init();
    void Shutdown();

    void SetVisible(bool v) {
        m_IsVisible = v;
    }
    bool IsVisible() const {
        return m_IsVisible;
    }

    void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const;

   private:
    void BuildBuffers();

   private:
    unsigned int m_Vao = 0;
    unsigned int m_Vbo = 0;
    int m_VertexCount = 0;

    bool m_IsVisible = true;

    std::unique_ptr<Shader> m_Shader;
};

}  // namespace nc::vis
