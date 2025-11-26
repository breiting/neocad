#include <neocad/vis/AxisRenderer.hpp>
#include <neocad/vis/Shader.hpp>
#include <vector>

#include "assets/unlit_frag.h"
#include "assets/unlit_vert.h"

namespace nc::vis {

AxisRenderer::~AxisRenderer() {
    Shutdown();
}

bool AxisRenderer::Init() {
    m_Shader = std::make_unique<Shader>(unlit_vert_glsl, unlit_frag_glsl);

    BuildBuffers();
    return true;
}

void AxisRenderer::Shutdown() {
    if (m_Vbo) {
        glDeleteBuffers(1, &m_Vbo);
        m_Vbo = 0;
    }
    if (m_Vao) {
        glDeleteVertexArrays(1, &m_Vao);
        m_Vao = 0;
    }
}

void AxisRenderer::BuildBuffers() {
    struct V {
        float x, y, z, r, g, b;
    };
    std::vector<V> v;

    auto PushLine = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        v.push_back({a.x, a.y, a.z, c.r, c.g, c.b});
        v.push_back({b.x, b.y, b.z, c.r, c.g, c.b});
    };

    const float len = 10.0f;
    const float head = 2.0f;
    const float width = 1.0f;

    glm::vec3 colX(1, 0.2, 0.2);
    glm::vec3 colY(0.2, 0.9, 0.2);
    glm::vec3 colZ(0.2, 0.5, 1.0);

    // X
    PushLine({0, 0, 0}, {len, 0, 0}, colX);
    PushLine({len, 0, 0}, {len - head, width, 0}, colX);
    PushLine({len, 0, 0}, {len - head, -width, 0}, colX);

    // Y
    PushLine({0, 0, 0}, {0, len, 0}, colY);
    PushLine({0, len, 0}, {width, len - head, 0}, colY);
    PushLine({0, len, 0}, {-width, len - head, 0}, colY);

    // Z
    PushLine({0, 0, 0}, {0, 0, len}, colZ);
    PushLine({0, 0, len}, {width, 0, len - head}, colZ);
    PushLine({0, 0, len}, {-width, 0, len - head}, colZ);

    m_VertexCount = (int)v.size();

    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);
    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(V), v.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void AxisRenderer::Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const {
    if (!m_IsVisible || !m_Vao)
        return;

    m_Shader->Bind();
    glm::mat4 mvp = proj * view * model;
    m_Shader->SetMat4("uMVP", mvp);

    glBindVertexArray(m_Vao);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, m_VertexCount);
}

}  // namespace nc::vis
