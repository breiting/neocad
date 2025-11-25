#pragma once
#include <neocad/domain/Registry.hpp>
#include <neocad/editor/ViewController.hpp>
#include <neocad/vis/Mesh.hpp>
#include <neocad/vis/Renderer.hpp>
#include <unordered_map>

namespace nc::vis {

class RenderingSystem {
   public:
    RenderingSystem(Renderer& r) : m_Renderer(r) {
    }

    void Update(domain::Registry& registry);
    void Render(const editor::ViewState& view);

   private:
    Renderer& m_Renderer;
    std::unordered_map<domain::Entity, std::shared_ptr<Mesh>> m_Meshes;
    std::unordered_map<domain::Entity, std::shared_ptr<Material>> m_Material;
};

}  // namespace nc::vis
