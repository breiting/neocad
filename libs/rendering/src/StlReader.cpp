#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <neocad/rendering/StlReader.hpp>
#include <sstream>
#include <string>

namespace stlviewer {

namespace {

// STL binary layout constants (see STL spec)
constexpr std::streamoff kStlBinaryHeaderSize = 80;
constexpr std::streamoff kStlBinaryTriangleSize = 50;  // 12 bytes normal + 36 bytes verts + 2 bytes attribute

enum class StlFormat { Binary, Ascii };

[[nodiscard]] std::streamoff GetFileSize(std::ifstream& file) {
    const auto currentPos = file.tellg();
    file.seekg(0, std::ios::end);
    const auto size = file.tellg();
    file.seekg(currentPos);
    return size;
}

/// \brief Tries to decide whether a file is binary or ASCII STL.
///
/// Heuristik:
/// 1. Dateigröße mit (80 + 4 + n * 50) vergleichen.
/// 2. Falls das nicht passt, als ASCII behandeln.
///    (Das deckt 99% der Fälle vernünftig ab.)
[[nodiscard]] StlFormat DetectFormat(std::ifstream& file) {
    file.clear();
    file.seekg(0, std::ios::beg);

    const std::streamoff fileSize = GetFileSize(file);

    // Read header + triangle count
    char header[static_cast<std::size_t>(kStlBinaryHeaderSize)]{};
    file.read(header, kStlBinaryHeaderSize);

    std::uint32_t triangleCount = 0;
    file.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));

    if (!file.good()) {
        file.clear();
        file.seekg(0, std::ios::beg);
        return StlFormat::Ascii;
    }

    const std::streamoff expectedSize = kStlBinaryHeaderSize + static_cast<std::streamoff>(sizeof(triangleCount)) +
                                        static_cast<std::streamoff>(triangleCount) * kStlBinaryTriangleSize;

    file.clear();
    file.seekg(0, std::ios::beg);

    if (expectedSize == fileSize) {
        return StlFormat::Binary;
    }

    // Heuristik: wenn Header mit "solid" beginnt und nicht exakt
    // zur Binary-Größe passt, ist ASCII sehr wahrscheinlich.
    const std::string headerStr(header, header + 5);
    if (headerStr == "solid") {
        return StlFormat::Ascii;
    }

    // Fallback: ASCII, wenn wir der Größe nicht trauen.
    return StlFormat::Ascii;
}

[[nodiscard]] bool LoadBinaryStl(std::ifstream& file, TriMesh& mesh) {
    mesh.Clear();

    file.clear();
    file.seekg(0, std::ios::beg);

    // Header
    char header[static_cast<std::size_t>(kStlBinaryHeaderSize)]{};
    file.read(header, kStlBinaryHeaderSize);

    // Triangle count
    std::uint32_t triangleCount = 0;
    file.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));
    if (!file.good()) {
        std::cerr << "[StlReader] Failed to read triangle count in binary STL.\n";
        return false;
    }

    // Reserve memory: each triangle -> 3 vertices, 3 indices
    mesh.vertices.reserve(static_cast<std::size_t>(triangleCount) * 3);
    mesh.indices.reserve(static_cast<std::size_t>(triangleCount) * 3);

    for (std::uint32_t i = 0; i < triangleCount; ++i) {
        float normal[3]{};
        float v1[3]{};
        float v2[3]{};
        float v3[3]{};
        std::uint16_t attributeByteCount = 0;

        file.read(reinterpret_cast<char*>(normal), sizeof(normal));
        file.read(reinterpret_cast<char*>(v1), sizeof(v1));
        file.read(reinterpret_cast<char*>(v2), sizeof(v2));
        file.read(reinterpret_cast<char*>(v3), sizeof(v3));
        file.read(reinterpret_cast<char*>(&attributeByteCount), sizeof(attributeByteCount));

        if (!file.good()) {
            std::cerr << "[StlReader] Unexpected EOF while reading binary STL.\n";
            return false;
        }

        const glm::vec3 normalVec{normal[0], normal[1], normal[2]};

        const auto addVertex = [&mesh, &normalVec](const float v[3]) {
            TriMesh::PureVertex vertex{};
            vertex.position = glm::vec3{v[0], v[1], v[2]};
            vertex.normal = normalVec;

            mesh.vertices.push_back(vertex);
            mesh.indices.push_back(static_cast<std::uint32_t>(mesh.vertices.size() - 1));
        };

        addVertex(v1);
        addVertex(v2);
        addVertex(v3);
    }

    return true;
}

[[nodiscard]] bool LoadAsciiStl(std::ifstream& file, TriMesh& mesh) {
    mesh.Clear();

    file.clear();
    file.seekg(0, std::ios::beg);

    std::string line;
    glm::vec3 currentNormal{0.0f, 0.0f, 0.0f};
    bool inFacet = false;
    std::size_t vertexCountInFacet = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "facet") {
            // facet normal nx ny nz
            std::string normalKeyword;
            ss >> normalKeyword >> currentNormal.x >> currentNormal.y >> currentNormal.z;
            inFacet = true;
            vertexCountInFacet = 0;
        } else if (token == "vertex" && inFacet) {
            glm::vec3 position{};
            ss >> position.x >> position.y >> position.z;

            TriMesh::PureVertex vertex{};
            vertex.position = position;
            vertex.normal = currentNormal;

            mesh.vertices.push_back(vertex);
            mesh.indices.push_back(static_cast<std::uint32_t>(mesh.vertices.size() - 1));
            ++vertexCountInFacet;
        } else if (token == "endfacet") {
            // robustness: ignore if not exactly 3 vertices
            inFacet = false;
            vertexCountInFacet = 0;
        }
    }

    if (!file.eof() && file.fail()) {
        std::cerr << "[StlReader] Error while reading ASCII STL.\n";
        return false;
    }

    return !mesh.vertices.empty();
}

}  // namespace

bool StlReader::LoadFromFile(const std::string& filePath, TriMesh& outMesh) const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[StlReader] Could not open file: " << filePath << '\n';
        return false;
    }

    const StlFormat format = DetectFormat(file);

    switch (format) {
        case StlFormat::Binary:
            return LoadBinaryStl(file, outMesh);
        case StlFormat::Ascii:
            return LoadAsciiStl(file, outMesh);
        default:
            std::cerr << "[StlReader] Unknown STL format for file: " << filePath << '\n';
            return false;
    }
}

}  // namespace stlviewer
