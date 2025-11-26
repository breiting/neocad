#pragma once
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ViewController.hpp>
#include <neocad/vis/DirectionalLight.hpp>
#include <neocad/vis/IRenderer.hpp>
#include <neocad/vis/Mesh.hpp>
#include <unordered_map>

namespace nc::vis {

class RenderingSystem {
   public:
    RenderingSystem(std::unique_ptr<IRenderer> r);

    void SetViewportSize(int w, int h);

    void Update(domain::Registry& registry);
    void Render(const glm::mat4& view, const glm::mat4& proj);

   private:
    std::unique_ptr<IRenderer> m_Renderer;
    std::shared_ptr<DirectionalLight> m_Light;
    std::unordered_map<domain::Entity, std::shared_ptr<Mesh>> m_Meshes;
    std::unordered_map<domain::Entity, std::shared_ptr<Material>> m_Material;
};

}  // namespace nc::vis
