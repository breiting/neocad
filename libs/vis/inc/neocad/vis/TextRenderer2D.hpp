#pragma once

#include <glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "GlyphAtlas.hpp"

namespace nc::vis {

class TextRenderer2D {
   public:
    /// Initialize with a glyph atlas (bitmap font atlas).
    void Init(const GlyphAtlas& atlas, float ascent, float descent, float lineGap);

    /// Start a new text batch (clears internal CPU buffer).
    void BeginBatch();

    /// Add one text string to the current batch.
    /// (x, y) is the baseline position in screen space.
    void AddText(const std::string& text, float x, float y, float scale, const glm::vec3& color);

    /// Upload batched vertices to GPU and draw them in a single draw call.
    void Flush(const glm::mat4& proj);

    /// Convenience wrapper for one-off text draw (internally Begin/Add/Flush).
    void DrawText(const std::string& text, float x, float y, float scale, const glm::vec3& color,
                  const glm::mat4& proj);

   private:
    struct Vertex {
        float x, y;     // position
        float u, v;     // UV
        float r, g, b;  // color
    };

    const GlyphAtlas* m_Atlas = nullptr;
    GLuint m_Vao = 0;
    GLuint m_Vbo = 0;
    GLuint m_Shader = 0;

    std::vector<Vertex> m_Vertices;  // CPU-side batch buffer

    void CreateShader();
    void CreateBuffers();

    float m_Ascent = 0.0f;
    float m_Descent = 0.0f;
    float m_LineGap = 0.0f;
    float m_StartX = 0.0f;
};

}  // namespace nc::vis
