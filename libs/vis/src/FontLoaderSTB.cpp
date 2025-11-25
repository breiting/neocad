#include <fstream>
#include <iostream>
#include <neocad/rendering/FontLoaderSTB.hpp>
#include <vector>

#include "stb_truetype.h"

namespace nc::vis {

bool FontLoaderSTB::LoadFromSTB(const uint8_t* data, float pixelHeight) {
    stbtt_fontinfo font{};
    if (!stbtt_InitFont(&font, data, 0))
        return false;

    float scale = stbtt_ScaleForPixelHeight(&font, pixelHeight);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    m_Ascent = ascent * scale;
    m_Descent = descent * scale;
    m_LineGap = lineGap * scale;

    m_Glyphs.clear();

    for (char c = 32; c < 127; ++c) {
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font, c, scale, scale, &x0, &y0, &x1, &y1);

        int w = x1 - x0;
        int h = y1 - y0;
        if (w <= 0 || h <= 0)
            continue;

        int bw, bh;
        unsigned char* bmp = stbtt_GetCodepointBitmap(&font, 0.0f, scale, c, &bw, &bh, nullptr, nullptr);
        if (!bmp)
            continue;

        // Sanity: w/h sollten mit bw/bh übereinstimmen
        if (bw != w || bh != h) {
            // zur Not trotzdem übernehmen
            w = bw;
            h = bh;
        }

        FontGlyph g;
        g.size = {static_cast<float>(w), static_cast<float>(h)};
        g.bearing = {static_cast<float>(x0), static_cast<float>(y0)};  // Box relativ zur Baseline

        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);
        g.advance = advanceWidth * scale;

        g.bitmap.assign(bmp, bmp + (w * h));
        stbtt_FreeBitmap(bmp, nullptr);

        m_Glyphs[c] = std::move(g);
    }

    return true;
}

bool FontLoaderSTB::LoadTTFMemory(const uint8_t* data, float pixelHeight) {
    if (!data) {
        std::cerr << "[FontLoaderSTB] ERROR: Invalid memory block.\n";
        return false;
    }
    return LoadFromSTB(data, pixelHeight);
}

bool FontLoaderSTB::LoadTTF(const std::string& fileName, float pixelHeight) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "[FontLoaderSTB] ERROR: Could not open TTF file: " << fileName << "\n";
        return false;
    }

    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return LoadFromSTB(buffer.data(), pixelHeight);
}

const FontGlyph* FontLoaderSTB::GetGlyph(char c) const {
    auto it = m_Glyphs.find(c);
    return (it != m_Glyphs.end()) ? &it->second : nullptr;
}

}  // namespace nc
