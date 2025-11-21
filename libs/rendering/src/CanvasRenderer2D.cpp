#include "neocad/rendering/CanvasRenderer2D.hpp"

#include <glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

namespace nc::rendering {

namespace {

// Simple 2D shader: position + solid color
constexpr const char* kVertexShaderSrc = R"(
#version 410 core
layout(location = 0) in vec2 inPos;
uniform mat4 uProjView;
void main()
{
    gl_Position = uProjView * vec4(inPos, 0.0, 1.0);
}
)";

constexpr const char* kFragmentShaderSrc = R"(
#version 410 core
out vec4 FragColor;
uniform vec4 uColor;
void main()
{
    FragColor = uColor;
}
)";

GLuint CompileShader(GLenum type, const char* src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint status = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(logLen));
        glGetShaderInfoLog(id, logLen, nullptr, log.data());
        std::cerr << "[CanvasRenderer2D] Shader compile error: " << log.data() << "\n";
    }
    return id;
}

GLuint LinkProgram(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint status = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(logLen));
        glGetProgramInfoLog(prog, logLen, nullptr, log.data());
        std::cerr << "[CanvasRenderer2D] Program link error: " << log.data() << "\n";
    }

    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

}  // namespace

// -------------------------------------------------------------
// CanvasRenderer2D
// -------------------------------------------------------------

void CanvasRenderer2D::Init() {
    if (m_Program != 0) return;  // already initialized

    GLuint vs = CompileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
    m_Program = LinkProgram(vs, fs);

    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);
}

void CanvasRenderer2D::Render(const core::NeoModel& model, const glm::mat4& projView) {
    if (m_Program == 0) return;  // Init() not called yet

    const auto& points = model.GetPoints();
    const auto& edges = model.GetEdges();

    glUseProgram(m_Program);
    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

    // Common uniform: projection * view
    GLint locPV = glGetUniformLocation(m_Program, "uProjView");
    glUniformMatrix4fv(locPV, 1, GL_FALSE, &projView[0][0]);

    // Setup vertex attrib once
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,  // location
                          2,  // vec2
                          GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    for (const auto& face : model.GetFaces()) {
        std::vector<glm::vec2> verts;
        for (auto pid : face.points) {
            verts.push_back(points[pid.value].pos);
        }

        // Fill Face
        const auto& col = ui::Colors::Nord2;
        GLint locColor = glGetUniformLocation(m_Program, "uColor");
        glUniform4f(locColor, col.r, col.g, col.b, 0.25f);  // Alpha 0.25

        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2), verts.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)verts.size());
    }

    // --- Draw edges as lines ---------------------------------
    if (!edges.empty()) {
        std::vector<glm::vec2> lineVerts;
        lineVerts.reserve(edges.size() * 2);

        for (const auto& e : edges) {
            if (e.a.value >= points.size() || e.b.value >= points.size()) continue;

            const auto& pa = points[e.a.value];
            const auto& pb = points[e.b.value];

            lineVerts.push_back(pa.pos);
            lineVerts.push_back(pb.pos);
        }

        if (!lineVerts.empty()) {
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(lineVerts.size() * sizeof(glm::vec2)),
                         lineVerts.data(), GL_DYNAMIC_DRAW);

            const auto& col = ui::Colors::Nord10;  // bluish for edges
            GLint locColor = glGetUniformLocation(m_Program, "uColor");
            glUniform4f(locColor, col.x, col.y, col.z, col.w);

            glLineWidth(1.5f);
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVerts.size()));
        }
    }

    // --- Draw points as dots ---------------------------------
    if (!points.empty()) {
        std::vector<glm::vec2> pointVerts;
        pointVerts.reserve(points.size());

        for (const auto& p : points) pointVerts.push_back(p.pos);

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(pointVerts.size() * sizeof(glm::vec2)), pointVerts.data(),
                     GL_DYNAMIC_DRAW);

        const auto& col = ui::Colors::Nord8;  // brighter for points
        GLint locColor = glGetUniformLocation(m_Program, "uColor");
        glUniform4f(locColor, col.x, col.y, col.z, col.w);

        glPointSize(6.0f);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pointVerts.size()));
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

}  // namespace nc::rendering
