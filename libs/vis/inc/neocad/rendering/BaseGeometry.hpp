#pragma once
#include <glm/glm.hpp>
#include <neocad/domain/Vertex.hpp>
#include <vector>

namespace nc::vis {

class BaseGeometry {
   public:
    virtual void Upload() = 0;
    virtual void Render() const = 0;

    BaseGeometry();
    virtual ~BaseGeometry() = default;

    void AddVertex(const domain::Vertex& v);
    void SetVertices(const std::vector<domain::Vertex>& vertices);
    size_t VertexCount() const;
    const std::vector<domain::Vertex>& GetVertices() const;

   protected:
    std::vector<domain::Vertex> m_Vertices;
    bool m_Dirty;
};
}  // namespace nc::vis
