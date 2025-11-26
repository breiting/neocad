#pragma once
#include <glm/glm.hpp>
#include <neocad/vis/Material.hpp>

namespace nc::vis {

class BaseGeometry;

class IRenderer {
   public:
    virtual ~IRenderer() = default;

    virtual void SetViewportSize(int w, int h) = 0;

    virtual void BeginFrame(const glm::mat4& view, const glm::mat4& proj) = 0;
    virtual void EndFrame() = 0;

    virtual void DrawMesh(const std::shared_ptr<BaseGeometry> mesh, std::shared_ptr<Material> mat,
                          const glm::mat4& model) = 0;
    virtual void DrawLineSet(const std::shared_ptr<BaseGeometry> lines, std::shared_ptr<Material> mat,
                             const glm::mat4& model) = 0;
    virtual void DrawPoints(const std::shared_ptr<BaseGeometry> points, std::shared_ptr<Material> mat,
                            const glm::mat4& model) = 0;

    virtual void ToggleWireframe() = 0;
};

}  // namespace nc::vis
