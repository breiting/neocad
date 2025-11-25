#pragma once
#include <glad.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>

#include "FontLoaderSTB.hpp"

namespace nc::vis {

class GlyphAtlas {
   public:
    struct GlyphUV {
        glm::vec2 size;     // Pixel size of glyph
        glm::vec2 bearing;  // offset from baseline
        float advance;      // advance in px
        glm::vec4 uv;       // UV rectangle in atlas [u0, v0, u1, v1]
    };

    bool BuildFrom(const FontLoaderSTB& font);
    GLuint GetTexture() const {
        return m_Texture;
    }
    const GlyphUV* GetGlyph(char c) const;

    float GetAscent() const {
        return m_Ascent;
    }

   private:
    GLuint m_Texture = 0;
    std::unordered_map<char, GlyphUV> m_Glyphs;
    float m_Ascent{0.0};
};

}  // namespace nc
