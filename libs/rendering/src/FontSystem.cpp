#include "neocad/rendering/FontSystem.hpp"

#include <filesystem>
#include <iostream>

#include "roboto_regular.h"

namespace nc::rendering {

bool FontSystem::LoadFromMemory(const uint8_t* data, float pixelHeight) {
    if (!m_Loader.LoadTTFMemory(data, pixelHeight)) {
        return false;
    }
    if (!m_Atlas.BuildFrom(m_Loader)) {
        return false;
    }
    return true;
}

std::string FontSystem::FindFont(const std::string& fileName) {
    const std::vector<std::string> locations = {"/System/Library/Fonts/", "/Library/Fonts/",
                                                std::string(getenv("HOME")) + "/Library/Fonts/"};

    for (const auto& dir : locations) {
        std::filesystem::path p = dir + fileName;
        if (std::filesystem::exists(p)) return p.string();
    }
    return "";
}

bool FontSystem::LoadFromFile(const std::string& path, float pixelHeight) {
    if (!m_Loader.LoadTTF(path, pixelHeight)) {
        std::cerr << "[FontSystem] ERROR: Failed to load TTF: " << path << "\n";
        return false;
    }
    if (!m_Atlas.BuildFrom(m_Loader)) {  // oder BuildMSDFCore(...) später
        std::cerr << "[FontSystem] ERROR: Failed to build atlas\n";
        return false;
    }
    std::cout << "[FontSystem] Loaded font from: " << path << "\n";
    return true;
}

bool FontSystem::LoadDefaultFont() {
    constexpr float PIXEL_HEIGHT = 48.0f;
    return LoadFromMemory(g_RobotoRegular, PIXEL_HEIGHT);
}

}  // namespace nc::rendering
