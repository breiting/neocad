#include <glm/gtc/type_ptr.hpp>
#include <neocad/vis/TextRenderer2D.hpp>

namespace nc::vis {

static const char* kTextVS = R"(
#version 410 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
layout(location=2) in vec3 aColor;

uniform mat4 uProj;

out vec2 vUV;
out vec3 vColor;

void main() {
    vUV = aUV;
    vColor = aColor;
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
}
)";

static const char* kTextFS = R"(
#version 410 core
in vec2 vUV;
in vec3 vColor;

out vec4 FragColor;

uniform sampler2D uTex;

void main() {
    float alpha = texture(uTex, vUV).r;
    if (alpha < 0.01)
        discard;
    FragColor = vec4(vColor, alpha);
}
)";

// -------------------------------------------------
// Internal helpers
// -------------------------------------------------
void TextRenderer2D::CreateShader() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &kTextVS, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &kTextFS, nullptr);
    glCompileShader(fs);

    m_Shader = glCreateProgram();
    glAttachShader(m_Shader, vs);
    glAttachShader(m_Shader, fs);
    glLinkProgram(m_Shader);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void TextRenderer2D::CreateBuffers() {
    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

    // Reserve some space – will be resized as needed
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertex) * 1024,  // initial, will grow
                 nullptr, GL_DYNAMIC_DRAW);

    // layout(location=0) vec2 aPos;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

    // layout(location=1) vec2 aUV;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

    // layout(location=2) vec3 aColor;
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));

    glBindVertexArray(0);
}

// -------------------------------------------------
// Public API
// -------------------------------------------------
void TextRenderer2D::Init(const GlyphAtlas& atlas, float ascent, float descent, float lineGap) {
    m_Atlas = &atlas;
    m_Ascent = ascent;
    m_Descent = descent;
    m_LineGap = lineGap;
    CreateShader();
    CreateBuffers();
    m_Vertices.reserve(2048);  // some default capacity
}

void TextRenderer2D::BeginBatch() {
    m_Vertices.clear();
}

void TextRenderer2D::AddText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    if (!m_Atlas)
        return;

    m_StartX = x;
    float penX = x;
    float baselineY = y;

    for (char c : text) {
        const auto* g = m_Atlas->GetGlyph(c);

        if (c == '\n') {
            penX = m_StartX;  // Reset X
            baselineY += (m_Ascent - m_Descent + m_LineGap) * scale;
            continue;
        }

        if (c == '\n') {
        } else if (!g || g->size.x <= 0.0f || g->size.y <= 0.0f) {
            // Glyph exists, but is blank (e.g. space) → still advance!
            penX += g ? (g->advance * scale) : (10.0f * scale);  // fallback advance
            continue;
        }

        float gw = g->size.x * scale;
        float gh = g->size.y * scale;

        // bearing (x0, y0) kommt aus stbtt_GetCodepointBitmapBox, relativ zur Baseline
        float xpos = penX + g->bearing.x * scale;
        float ypos = baselineY + g->bearing.y * scale;

        float x0 = xpos;
        float y0 = ypos;
        float x1 = xpos + gw;
        float y1 = ypos + gh;

        float u0 = g->uv.x;
        float v0 = g->uv.y;
        float u1 = g->uv.z;
        float v1 = g->uv.w;

        // Zwei Dreiecke (CCW)
        Vertex v[6] = {{x0, y0, u0, v0, color.r, color.g, color.b}, {x1, y0, u1, v0, color.r, color.g, color.b},
                       {x1, y1, u1, v1, color.r, color.g, color.b},

                       {x0, y0, u0, v0, color.r, color.g, color.b}, {x1, y1, u1, v1, color.r, color.g, color.b},
                       {x0, y1, u0, v1, color.r, color.g, color.b}};

        m_Vertices.insert(m_Vertices.end(), std::begin(v), std::end(v));

        penX += g->advance * scale;
    }
}

void TextRenderer2D::Flush(const glm::mat4& proj) {
    if (!m_Atlas || m_Vertices.empty())
        return;

    glUseProgram(m_Shader);
    glUniformMatrix4fv(glGetUniformLocation(m_Shader, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Atlas->GetTexture());
    glUniform1i(glGetUniformLocation(m_Shader, "uTex"), 0);

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

    // Upload full batch in one go
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_Vertices.size()));

    glBindVertexArray(0);
}

void TextRenderer2D::DrawText(const std::string& text, float x, float y, float scale, const glm::vec3& color,
                              const glm::mat4& proj) {
    BeginBatch();
    AddText(text, x, y, scale, color);
    Flush(proj);
}

}  // namespace nc::vis
