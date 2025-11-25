#pragma once
#include <glm/glm.hpp>
#include <neocad/rendering/Vertex.hpp>
#include <vector>

namespace nc::vis {

class BaseGeometry {
   public:
    virtual void Upload() = 0;
    virtual void Render() const = 0;

    BaseGeometry();
    virtual ~BaseGeometry() = default;

    void AddVertex(const Vertex& v);
    size_t VertexCount() const;
    const std::vector<Vertex>& GetVertices() const;

   protected:
    std::vector<Vertex> m_Vertices;
    bool m_Dirty;
};
}  // namespace nc
