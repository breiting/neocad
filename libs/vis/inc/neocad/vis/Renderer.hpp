#pragma once
#include <glm/mat4x4.hpp>
#include <memory>
#include <neocad/vis/Light.hpp>
#include <neocad/vis/LineSet.hpp>
#include <neocad/vis/Material.hpp>
#include <neocad/vis/Mesh.hpp>
#include <neocad/vis/PointCloud.hpp>

namespace nc::vis {

/// Minimal high-level renderer facade.
/// - Holds current view/projection (from ICamera)
/// - Uses a Material to set shader state
/// - Delegates geometry drawing to Mesh / LineSet / Point
class Renderer {
   public:
    Renderer();
    /// Must be called once per frame before Draw*.
    void BeginFrame(const glm::mat4& view, const glm::mat4& proj);

    /// Draw solid triangle mesh.
    void DrawMesh(const Mesh& mesh, std::shared_ptr<Material> mat, const glm::mat4& model);

    /// Draw line set.
    void DrawLineSet(const LineSet& lines, std::shared_ptr<Material> mat, const glm::mat4& model);

    /// Draw points.
    void DrawPoints(const PointCloud& points, std::shared_ptr<Material> mat, const glm::mat4& model);

    void EndFrame();

   private:
    glm::mat4 m_View{1.0f};
    glm::mat4 m_Proj{1.0f};
    std::shared_ptr<Light> m_Light;
};

}  // namespace nc::vis
