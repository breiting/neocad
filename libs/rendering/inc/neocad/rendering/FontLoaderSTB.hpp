#pragma once
#include <cstdint>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>

namespace nc {

struct FontGlyph {
    glm::vec2 size;               // width/height in px
    glm::vec2 bearing;            // offset from baseline
    float advance;                // advance in px
    std::vector<uint8_t> bitmap;  // alpha bitmap (w*h)
};

class FontLoaderSTB {
   public:
    bool LoadTTFMemory(const uint8_t* data, float pixelHeight);
    bool LoadTTF(const std::string& fileName, float pixelHeight);
    const FontGlyph* GetGlyph(char c) const;

    float GetAscent() const {
        return m_Ascent;
    }
    float GetDescent() const {
        return m_Descent;
    }
    float GetLineGap() const {
        return m_LineGap;
    }

   private:
    bool LoadFromSTB(const unsigned char* data, float pixelHeight);

   private:
    std::unordered_map<char, FontGlyph> m_Glyphs;
    float m_Ascent = 0.f;
    float m_Descent = 0.f;
    float m_LineGap = 0.f;
};

}  // namespace nc
