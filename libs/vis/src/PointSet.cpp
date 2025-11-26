// clang-format off
#include <glad.h>
// clang-format on
#include <glm/gtc/constants.hpp>
#include <neocad/vis/PointSet.hpp>
#include <neocad/vis/Shader.hpp>

using namespace nc::domain;

namespace nc::vis {

PointSet::PointSet() {
}

PointSet::~PointSet() {
    deleteBuffers();
}

void PointSet::deleteBuffers() {
    if (m_Vbo) {
        glDeleteBuffers(1, &m_Vbo);
        m_Vbo = 0;
    }
    if (m_Vao) {
        glDeleteVertexArrays(1, &m_Vao);
        m_Vao = 0;
    }
}

void PointSet::Upload() {
    if (m_Vertices.empty())
        return;

    if (!m_Vao) {
        glGenVertexArrays(1, &m_Vao);
        glGenBuffers(1, &m_Vbo);
    }

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::PositionOffset());

    // Instancing !
    glVertexAttribDivisor(0, 1);

    glBindVertexArray(0);
    m_Dirty = false;
}

void PointSet::Render() const {
    if (!m_Vao)
        return;

    glBindVertexArray(m_Vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(m_Vertices.size()));
    glBindVertexArray(0);
}

}  // namespace nc::vis
