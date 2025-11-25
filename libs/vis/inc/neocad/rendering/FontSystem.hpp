#pragma once
#include "FontLoaderSTB.hpp"
#include "GlyphAtlas.hpp"

namespace nc::vis {

class FontSystem {
   public:
    bool LoadFromMemory(const uint8_t* data, float pixelHeight);
    bool LoadFromFile(const std::string& path, float pixelHeight);
    bool LoadDefaultFont();

    static std::string FindFont(const std::string& fileName);

    const GlyphAtlas& GetAtlas() const {
        return m_Atlas;
    }

    float GetAscent() const {
        return m_Loader.GetAscent();
    }
    float GetDescent() const {
        return m_Loader.GetDescent();
    }
    float GetLineGap() const {
        return m_Loader.GetLineGap();
    }

   private:
    FontLoaderSTB m_Loader;
    GlyphAtlas m_Atlas;
};

}  // namespace nc::vis
