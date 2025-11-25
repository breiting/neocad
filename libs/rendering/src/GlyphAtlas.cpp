#include <neocad/rendering/GlyphAtlas.hpp>
#include <vector>

namespace nc {

bool GlyphAtlas::BuildFrom(const FontLoaderSTB& font) {
    const int atlasSize = 2048;
    const int padding = 2;

    std::vector<unsigned char> atlas(atlasSize * atlasSize, 0);

    int penX = padding;
    int penY = padding;
    int rowHeight = 0;

    m_Glyphs.clear();
    m_Ascent = font.GetAscent();

    for (char c = 32; c < 127; ++c) {
        const FontGlyph* g = font.GetGlyph(c);
        if (!g)
            continue;

        int gw = static_cast<int>(g->size.x);
        int gh = static_cast<int>(g->size.y);

        if (penX + gw + padding > atlasSize) {
            penX = padding;
            penY += rowHeight + padding;
            rowHeight = 0;
        }
        if (penY + gh + padding > atlasSize) {
            break;  // atlas voll, für MVP ok
        }

        for (int y = 0; y < gh; ++y) {
            for (int x = 0; x < gw; ++x) {
                int src = y * gw + x;
                int dstX = penX + x;
                int dstY = penY + y;
                int dst = dstY * atlasSize + dstX;
                atlas[dst] = g->bitmap[src];
            }
        }

        GlyphUV info;
        info.size = g->size;
        info.bearing = g->bearing;
        info.advance = g->advance;

        info.uv = glm::vec4(static_cast<float>(penX) / atlasSize, static_cast<float>(penY) / atlasSize,
                            static_cast<float>(penX + gw) / atlasSize, static_cast<float>(penY + gh) / atlasSize);

        m_Glyphs[c] = info;

        penX += gw + padding;
        if (gh > rowHeight)
            rowHeight = gh;
    }

    if (m_Texture) {
        glDeleteTextures(1, &m_Texture);
        m_Texture = 0;
    }

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlasSize, atlasSize, 0, GL_RED, GL_UNSIGNED_BYTE, atlas.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

const GlyphAtlas::GlyphUV* GlyphAtlas::GetGlyph(char c) const {
    auto it = m_Glyphs.find(c);
    return (it != m_Glyphs.end()) ? &it->second : nullptr;
}

}  // namespace nc
